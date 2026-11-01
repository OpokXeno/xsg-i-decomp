#!/usr/bin/env python3
"""Compile ordinary IOP C and require fresh complete SSD/RSSD IRX exactness.

CLI follows match.py: --root PROJECT --packet JSON --output-dir FRESH --profile
JSON. Similarity/disassembly is diagnostic. Exactness additionally requires
compiler emission, original relocation/addend identity and the whole-module gate.
"""
import argparse
from collections import Counter
import difflib
import json
import os
from pathlib import Path
import re
import shutil
import signal
import subprocess
import sys

sys.dont_write_bytecode = True
BASE = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(BASE))
from tools.iop_backend import (ELF, PROFILE, REVISION, BINDINGS, address, digest, difference,
                               emission_audit, identifier, ordinary_c, read_json, relative,
                               require, scoped_relocations)
from tools.iop_inventory import verify_iop
from tools.tu_manifest import debug_records


def original_span(module, data, start, size):
    sections = [s for s in module["sections"] if s["flags"]&2 and s["module_offset"] <= start
                and start+size <= s["module_offset"]+s["size"]]
    require(len(sections)==1,"original span has no unique allocated section")
    section = sections[0]
    offset = section["file_offset"]+start-section["module_offset"]
    return section, None if section["type"]==8 else data[offset:offset+size]


def resolve_original_object(root, profile, packet):
    """Resolve one original ECOFF object; packet ownership labels are not authority."""
    policy = profile.get("original_object_modes")
    require(isinstance(policy, dict) and set(policy) == {"schema", "default_mode", "units"}
            and policy["schema"] == "iop-original-object-float/1"
            and policy["default_mode"] == "soft"
            and set(policy["units"]) == {"iop_ssd", "iop_rssd"},
            "unsupported original-object IOP float policy")
    require(isinstance(packet, dict) and packet.get("unit") in policy["units"],
            "original-object IOP contract requires an original function selector")
    root = Path(root).resolve()
    inventory = verify_iop(root)
    for unit, assignment in policy["units"].items():
        require(isinstance(assignment, dict)
                and set(assignment) == {"binary_sha256", "hard_fdrs"}
                and assignment["binary_sha256"] == inventory["units"][unit]["sha256"]
                and isinstance(assignment["hard_fdrs"], list),
                "original-object IOP policy binary pin differs")
    unit = packet["unit"]
    module = inventory["units"][unit]
    binary = (packet.get("binaries") or {}).get(unit) or {}
    require(binary.get("sha256") == module["sha256"],
            "IOP float selector original binary SHA differs")
    path = root / "game/extracted-iso" / module["path"]
    data = path.read_bytes()
    require(digest(data) == module["sha256"], "IOP original changed during object resolution")
    section = next(s for s in module["sections"] if s["name"] == ".mdebug")
    decoder = root / "tools/tu_manifest.py"
    require(decoder.is_file()
            and digest(decoder) == digest(Path(debug_records.__code__.co_filename)),
            "IOP ECOFF decoder differs from the pinned private snapshot")
    records = debug_records(data, section)["records"]
    hard = {}
    for assignment in policy["units"][unit]["hard_fdrs"]:
        require(isinstance(assignment, dict) and set(assignment) == {"index", "filename"}
                and type(assignment["index"]) is int and assignment["index"] >= 0
                and isinstance(assignment["filename"], str)
                and assignment["index"] not in hard, "invalid or duplicate hard IOP FDR")
        matching = [r for r in records if r["index"] == assignment["index"]
                    and r["source"] == assignment["filename"]]
        require(len(matching) == 1, "hard IOP FDR filename/index differs from original ECOFF")
        hard[assignment["index"]] = assignment["filename"]
    functions = packet.get("functions")
    data_fdr = packet.get("data_fdr")
    if functions == [] and data_fdr is not None:
        # A data-only original object (e.g. ssd_data.c) has an FDR without
        # procedures; it is selected by its exact original index and filename.
        require(isinstance(data_fdr, dict) and set(data_fdr) == {"index", "filename"},
                "invalid data-only IOP FDR selector")
        matching = [r for r in records if r["index"] == data_fdr["index"]
                    and r["source"] == data_fdr["filename"]]
        require(len(matching) == 1 and not any(s["kind"] in (6, 14) for s in matching[0]["symbols"]),
                "data-only IOP FDR differs from original ECOFF or has procedures")
        record = matching[0]
        return dict(schema="iop-original-object/1", unit=unit, binary_sha256=module["sha256"],
                    fdr_index=record["index"], filename=record["source"],
                    fdr_file_offset=record["fdr_offset"],
                    float_mode="hard" if record["index"] in hard else "soft",
                    mdebug_sha256=section["sha256"],
                    policy_sha256=digest(json.dumps(policy, sort_keys=True, separators=(",", ":")).encode()),
                    functions=[])
    require(isinstance(functions, list) and functions,
            "IOP object selector has no original functions")
    resolved, seen = [], set()
    for function in functions:
        name = function.get("name")
        require(isinstance(name, str), "IOP object selector lacks an original function name")
        identifier(name)
        va = address(function["va"])
        require((name, va) not in seen, "duplicate IOP object function selector")
        seen.add((name, va))
        elf = [s for s in module["symbols"] if s["type"] == "FUNC"
               and s["name"] == name and s["value"] == va]
        require(len(elf) == 1, "IOP object selector is not an original ELF function")
        matches = [(r, s) for r in records for s in r["symbols"]
                   if s["kind"] in (6, 14) and s["name"] == name and s["address"] == va]
        require(len(matches) == 1, "IOP original ECOFF function ownership is missing or ambiguous")
        record, symbol = matches[0]
        resolved.append(dict(name=name, va=f"0x{va:08x}", fdr_index=record["index"],
                             filename=record["source"], fdr_file_offset=record["fdr_offset"],
                             symbol_file_offset=symbol["file_offset"]))
    require(len({item["fdr_index"] for item in resolved}) == 1,
            "IOP source allocation crosses original ECOFF objects")
    first = resolved[0]
    return dict(schema="iop-original-object/1", unit=unit, binary_sha256=module["sha256"],
                fdr_index=first["fdr_index"], filename=first["filename"],
                fdr_file_offset=first["fdr_file_offset"],
                float_mode="hard" if first["fdr_index"] in hard else "soft",
                mdebug_sha256=section["sha256"],
                policy_sha256=digest(json.dumps(policy, sort_keys=True, separators=(",", ":")).encode()),
                functions=resolved)


def contract(root, profile_path, packet=None):
    root = Path(root).resolve()
    profile = read_json(profile_path)
    require(profile.get("schema_version")==1 and profile.get("selected")==PROFILE
            and profile.get("revision")==REVISION, "unsupported IOP profile")
    require(profile.get("default_flags")==["-O2","-G0"]
            and profile.get("permitted_flags")==[["-O2","-G0"]]
            and profile.get("source_languages")==["c"] and profile.get("timeout_seconds")==30,
            "IOP compiler flags/language/timeout contract differs")
    require(profile.get("address_kind")=="module_relative"
            and profile.get("gate")=="complete_irx_and_compiler_emission"
            and set(profile.get("units",[]))=={"iop_ssd","iop_rssd"},"IOP module/gate contract differs")
    lock_path = root/relative(profile["compiler_lock"])
    require(digest(lock_path)==profile["compiler_lock_sha256"],"IOP toolchain lock changed")
    lock = read_json(lock_path)
    require(lock.get("schema_version")==1 and lock.get("profile")==PROFILE
            and lock.get("revision")==REVISION,"IOP toolchain identity differs")
    variants = [v for v in lock["variants"] if v["id"]==PROFILE]
    require(len(variants)==1,"IOP compiler variant is not unique")
    variant = variants[0]
    command = variant["command"]
    require(len(command)==8 and command[1].startswith("-B") and command[2:]==[
        "-nostdinc","-fno-builtin","-mips1","-msoft-float","-fno-pic","-S"]
        and variant["flags"]==["-O2","-G0"],"unsupported IOP compiler command")
    if "original_object_modes" in profile:
        original_object = resolve_original_object(root, profile, packet)
        modes = variant.get("command_by_float_mode")
        require(isinstance(modes, dict) and set(modes) == {"soft", "hard"},
                "IOP original-object contract lacks its closed soft/hard commands")
        hard = [*command[:5], "-mhard-float", *command[6:]]
        require(modes["soft"] == command and modes["hard"] == hard,
                "IOP float commands differ beyond the calibrated explicit mode flag")
        command = modes[original_object["float_mode"]]
    else:
        require("command_by_float_mode" not in variant,
                "IOP float commands require a pinned original-object policy")
    resolved = [str(root/relative(command[0])),"-B"+str(root/relative(command[1][2:].rstrip("/")))+"/",*command[2:]]
    tools = {name:str(root/relative(path)) for name,path in lock["tools"].items()}
    require({"assembler","linker","objdump","readelf"}<=tools.keys(),"incomplete IOP tools")
    assembler_flags = lock.get("assembler_flags",profile.get("assembler_flags"))
    require(assembler_flags==["-EL","-mips1","-32","-G0","-O2"],"unapproved IOP assembler flags")
    pins = {str(root/relative(pin["path"])):pin["sha256"] for pin in lock["pins"]}
    required = {resolved[0],*tools.values()}
    support = Path(resolved[1][2:].rstrip("/"))
    required.update(str(support/name) for name in ("cc1","cpp","as","specs"))
    require(required<=pins.keys(),"IOP toolchain payload lacks pins")
    for name,expected in pins.items():
        require(Path(name).is_file() and digest(name)==expected,"IOP toolchain payload changed: "+name)
    if "original_object_modes" in profile:
        decoder = root / "tools/tu_manifest.py"
        require(decoder.is_file(), "IOP original-object decoder missing from pinned snapshot")
        pins[str(decoder)] = digest(decoder)
    return profile,lock,lock_path,resolved,tools,assembler_flags,pins


def validate_packet(root, packet, profile, module, original_data):
    required = {"schema_version","revision","unit","binaries","source","headers","text",
                "functions","references","generated_data"}
    optional = {"address_kind","flags","source_language"}
    require(set(packet)-optional==required and packet["schema_version"]==1 and packet["revision"]==REVISION,
            "IOP packet differs from strict schema1")
    require(packet.get("address_kind","module_relative")=="module_relative"
            and packet.get("source_language","c")=="c","IOP matching accepts module-relative ordinary C only")
    require(packet.get("flags",profile["default_flags"])==["-O2","-G0"],"packet changes IOP compiler flags")
    unit = packet["unit"]
    require(packet["binaries"]=={unit:dict(path="game/extracted-iso/"+module["path"],sha256=module["sha256"])},
            "IOP packet binary identity differs or aliases an EE unit")
    start,size = address(packet["text"]["va"]),packet["text"]["size"]
    require(type(size) is int and 0<size<=1048576 and start%4==size%4==0,"invalid full IOP text extent")
    original_section,expected = original_span(module,original_data,start,size)
    require(original_section["flags"]&4 and digest(expected)==packet["text"]["sha256"],"original IOP text span pin differs")
    names,cursor = set(),start
    for function in packet["functions"]:
        identifier(function["name"])
        value=address(function["va"])
        require(function["name"] not in names and value>=cursor and type(function["size"]) is int
                and function["size"]>0 and function["size"]%4==0,"invalid or overlapping IOP functions")
        matches=[s for s in module["symbols"] if s["type"]=="FUNC" and s["name"]==function["name"]
                 and s["value"]==value and s["size"]==function["size"]]
        require(len(matches)==1,"IOP function identity differs from original")
        cursor=value+function["size"]
        names.add(function["name"])
    require(names and cursor<=start+size,"full text omits complete IOP function extent")
    inside={s["name"] for s in module["symbols"] if s["type"]=="FUNC" and s["size"]>0
            and s["value"]<start+size and start<s["value"]+s["size"]}
    require(inside==names,"IOP packet omits an original function/alias in claimed text")
    spans=[dict(packet["text"],section=original_section["name"])]
    expected_spans=[expected]
    ranges=[(start,start+size)]
    sections=set()
    for span in packet["generated_data"]:
        lo,n=address(span["va"]),span["size"]
        require(span["section"] in (".rodata",".data",".sdata",".lit4",".lit8",".bss",".sbss")
                and span["section"] not in sections and type(n) is int and 0<n<=1048576
                and not any(lo<b and a<lo+n for a,b in ranges),"invalid generated IOP data scope")
        section,data=original_span(module,original_data,lo,n)
        require(section["name"]==span["section"] and not section["flags"]&4
                and span["sha256"]==(digest(data) if data is not None else None),"generated IOP data pin differs")
        spans.append(span)
        expected_spans.append(data)
        sections.add(span["section"])
        ranges.append((lo,lo+n))
    bindings=[]
    for name,reference in packet["references"].items():
        identifier(name)
        require(name not in names and reference["unit"]==unit,"cross-module or function-colliding IOP reference")
        lo,n=address(reference["va"]),reference["size"]
        require(type(n) is int and 0<=n<=1048576,"invalid IOP reference extent")
        symbols=[s for s in module["symbols"] if s["name"]==name and s["type"]!="SECTION"
                 and s["value"]==lo and s["section_index"]!=0]
        require(len(symbols)==1,"IOP reference is not an original named symbol")
        symbol=symbols[0]
        all_names=sorted({s["name"] for s in module["symbols"] if s["name"] and s["value"]==lo
                          and s["type"]!="SECTION" and s["section_index"]!=0})
        require(reference["symbols"]==all_names,"IOP reference alias witness differs")
        if n==0:
            require(symbol["type"]=="OBJECT" and symbol["size"]==0,"zero reference is not original OBJECT metadata")
            require(0<symbol["section_index"]<len(module["sections"]),"metadata reference lacks original section")
            section=module["sections"][symbol["section_index"]]
            data=None if section["type"]==8 else b""
        else:
            section,data=original_span(module,original_data,lo,n)
        require(reference.get("sha256")== (digest(data) if data is not None else None),"IOP reference byte witness differs")
        require(not any(lo<b and a<lo+n for a,b in ranges),"IOP reference overlaps generated source scope")
        bindings.append(dict(name=name,va=reference["va"],size=n,verified=True,section=section["name"],
                             original_symbol=symbol,scaffolding=True,recovered_data=False))
    return spans,expected_spans,bindings


def match(root,packet_path,output,profile_path):
    root,packet_path,output,profile_path=map(lambda p:Path(p).resolve(),(root,packet_path,output,profile_path))
    require(not output.exists() and output.is_relative_to(root) and
            output.parts[len(root.parts)] in ("build",".work"),"IOP output must be a fresh private build directory")
    packet=read_json(packet_path)
    profile,lock,lock_path,compiler,tools,assembler_flags,inputs=contract(root,profile_path,packet)
    require(packet.get("unit") in ("iop_ssd","iop_rssd"),"unknown IOP unit")
    inventory=verify_iop(root)
    module=inventory["units"][packet["unit"]]
    original_path=root/"game/extracted-iso"/module["path"]
    original_data=original_path.read_bytes()
    original=ELF(original_data)
    spans,expected,bindings=validate_packet(root,packet,profile,module,original_data)
    inputs.update({str(p):digest(p) for p in (packet_path,profile_path,lock_path,Path(__file__),
        root/"tools/iop_backend.py",root/"tools/iop_inventory.py",root/"config/iop-targets.json",
        root/"config/iop-tools.lock.json",root/"config/targets.json",original_path)})
    files=[packet["source"],*packet["headers"]]
    stages=[relative(f["staged_path"]) for f in files]
    require(stages[0].suffix==".c" and all(p.suffix==".h" for p in stages[1:])
            and len(set(stages))==len(stages),"invalid IOP staged source/header paths")
    for entry,stage in zip(files,stages):
        path=root/relative(entry["path"])
        require(path.is_file() and path.resolve().is_relative_to(root) and digest(path)==entry["sha256"],
                "IOP candidate source/header hash or path differs")
        ordinary_c(path.read_text(encoding="ascii"),stage,{p.as_posix() for p in stages[1:]})
        inputs[str(path)]=entry["sha256"]
    output.mkdir(parents=True)
    for entry,stage in zip(files,stages):
        dest=output/"staged"/stage
        dest.parent.mkdir(parents=True,exist_ok=True)
        shutil.copyfile(root/entry["path"],dest)
        require(digest(dest)==entry["sha256"],"IOP source changed during staging")
        inputs[str(dest)]=entry["sha256"]
    shutil.copyfile(packet_path,output/"packet.json")
    result=dict(schema_version=1,revision=REVISION,unit=packet["unit"],address_kind="module_relative",
                binary=packet["binaries"][packet["unit"]],source_language="c",recovery_kind="c",semantic_acceptance=False,
                packet_sha256=digest(packet_path),profile=profile,flags=["-O2","-G0"],input_hashes=inputs,
                commands=[],bindings=bindings,functions=[],spans=[],unresolved=[],pending_relocations=[],
                extra_sections=[],extra_functions=[],gate_passed=False,code_bytes_verified=False,
                data_generated_checked=False,external_data_witnessed=True,unit_verified=False,
                source_emission=dict(verified=False),relocations_equal=False,
                source_eligibility=dict(verified=False,authored_and_headers_checked=True,
                                        preprocessed_checked=False,language="c",assembly_admitted=False,
                                        register_bindings_admitted=False),
                module_reconstruction=dict(gate_passed=False,whole_file_equal=False),
                original_compiler_distribution_proven=False,external_data_compiled=False,
                generated_data_bytes=sum(s["size"] for s in packet["generated_data"]))
    if "original_object_modes" in profile:
        result["original_object"] = resolve_original_object(root, profile, packet)
    env=dict(PATH="/usr/bin:/bin",LC_ALL="C",LANG="C",HOME=str(output),TMPDIR=str(output))

    def run(argv,label):
        for path,expected_hash in inputs.items():
            require(digest(path)==expected_hash,"IOP input changed before subprocess: "+path)
        log=output/(label+".log")
        with log.open("xb") as stream:
            proc=subprocess.Popen([str(a) for a in argv],cwd=output,env=env,stdout=stream,
                                  stderr=subprocess.STDOUT,start_new_session=True)
            try:
                rc=proc.wait(timeout=30)
            except subprocess.TimeoutExpired:
                os.killpg(proc.pid,signal.SIGKILL)
                proc.wait()
                rc=124
        result["commands"].append(dict(argv=list(map(str,argv)),cwd=str(output),returncode=rc,
                                         log=log.name,log_sha256=digest(log)))
        return rc

    def compare_functions(candidate):
        checks=[]
        for function in packet["functions"]:
            start=address(function["va"])
            _,before=original_span(module,original_data,start,function["size"])
            symbols=[s for s in candidate.symbols if s["name"]==function["name"] and s["type"]==2
                     and s["section_index"] not in (0,0xfff1)]
            symbol=symbols[0] if len(symbols)==1 else None
            after=candidate.function_bytes(symbol) if symbol else b""
            reference=next(s for s in module["symbols"] if s["name"]==function["name"] and s["type"]=="FUNC")
            binding_equal=bool(symbol and symbol["binding"]==BINDINGS[reference["binding"]]
                               and symbol["other"]==reference["other"])
            checks.append(dict(function,**difference(before,after,start),binding_equal=binding_equal,
                               expected_binding=BINDINGS[reference["binding"]],actual_binding=symbol["binding"] if symbol else None,
                               identity_equal=bool(symbol and symbol["value"]==start and symbol["size"]==function["size"]),
                               compiler_emitted=any(c["name"]==function["name"] and c["compiler_emitted"]
                                   for c in result["source_emission"].get("functions",[])),verified=False))
        return checks

    try:
        includes=["-Istaged","-Istaged/include"]
        source="staged/"+stages[0].as_posix()
        flags=["-O2","-G0"]
        require(run([*compiler[:-1],"-E",*flags,*includes,source,"-o","candidate.i"],"preprocess")==0,"IOP preprocessing failed")
        ordinary_c((output/"candidate.i").read_text(encoding="ascii"))
        result["source_eligibility"].update(verified=True,preprocessed_checked=True)
        inputs[str(output/"candidate.i")]=digest(output/"candidate.i")
        require(run([*compiler,*flags,"candidate.i","-o","candidate.s"],"compile")==0,"IOP compilation failed")
        assembly=(output/"candidate.s").read_text(encoding="ascii")
        inputs[str(output/"candidate.s")]=digest(output/"candidate.s")
        require(run([tools["assembler"],*assembler_flags,"-o","candidate.o","candidate.s"],"assemble")==0,"IOP assembly failed")
        inputs[str(output/"candidate.o")]=digest(output/"candidate.o")
        obj=ELF((output/"candidate.o").read_bytes())
        emission=emission_audit(obj,assembly,packet["functions"],module["symbols"],packet["generated_data"])
        emission.update(source_sha256=packet["source"]["sha256"],
                        preprocessed_sha256=digest(output/"candidate.i"),
                        compiler_assembly_sha256=digest(output/"candidate.s"),
                        compiler_object_sha256=digest(output/"candidate.o"),
                        ordinary_c_eligible=result["source_eligibility"]["verified"])
        result["source_emission"]=emission
        result["extra_functions"]=emission["extra_functions"]
        result["extra_sections"]=emission["extra_sections"]
        undefined=sorted({s["name"] for s in obj.symbols if s["name"] and s["section_index"]==0})
        result["unresolved"]=[name for name in undefined if name not in packet["references"]]
        # The standalone link is a diagnostic and an independent relocation/body
        # witness. The whole-module builder composes genuine compiler assembly,
        # so standalone assembler tail padding is never trimmed or substituted.
        script='OUTPUT_FORMAT("elf32-littlemips")\nOUTPUT_ARCH(mips:3000)\nSECTIONS {\n'
        script+=f'.text {packet["text"]["va"]} : SUBALIGN(4) {{ *(.text .text.*) }}\n'
        for span in packet["generated_data"]:
            common=" COMMON" if span["section"]==".bss" else ""
            script+=f'{span["section"]} {span["va"]} : {{ *({span["section"]} {span["section"]}.*{common}) }}\n'
        script+='}\n'
        for name,ref in packet["references"].items():
            script+=f'{name} = {ref["va"]};\n'
        (output/"layout.ld").write_text(script,encoding="ascii")
        link_rc=run([tools["linker"],"-m","elf32elmip","--emit-relocs","--no-undefined","-e",packet["functions"][0]["va"],
                     "-T","layout.ld","-o","candidate.elf","candidate.o"],"link")
        require(link_rc==0 and not result["unresolved"],"IOP diagnostic link failed or has undeclared external symbols")
        candidate=ELF((output/"candidate.elf").read_bytes())
        result["functions"]=compare_functions(candidate)
        scoped=[(s["section"],address(s["va"]),s["size"]) for s in spans]
        original_rels=scoped_relocations(original,scoped)
        diagnostic_rels=scoped_relocations(candidate,scoped)
        fields=("target_section","module_offset","type","word","explicit_addend")
        reloc_key=lambda r:tuple(r[k] for k in fields)
        # ET_EXEC fixups can list a pending HI16 after an intervening call;
        # the IRX emitter groups its native HI16/LO16 fixups at the HI address.
        # Compare every diagnostic fixup's exact identity and multiplicity, and
        # report stream order separately. The final IRX records below are still
        # compared in raw order, including r_info and in-place words, and the
        # complete file must be byte-identical. No artifact bytes are changed.
        original_keys=list(map(reloc_key,original_rels))
        diagnostic_keys=list(map(reloc_key,diagnostic_rels))
        result["relocation_comparison"]=dict(original=original_rels,candidate=diagnostic_rels,
                                              diagnostic_equal=Counter(original_keys)==Counter(diagnostic_keys),
                                              diagnostic_stream_order_equal=original_keys==diagnostic_keys,
                                              final_irx_raw_order_required=True)
        for label,binary,section in [("original",original_path,next(s["name"] for s in module["sections"] if s["flags"]&4)),
                                     ("candidate",output/"candidate.elf",".text")]:
            stop=address(packet["text"]["va"])+packet["text"]["size"]
            if label=="candidate":
                stop=max(stop,max((s["address"]+s["size"] for s in candidate.sections if s["flags"]&4),default=stop))
            require(run([tools["objdump"],"-Dz","-M","no-aliases","-j",section,
                         "--start-address="+packet["text"]["va"],"--stop-address="+hex(stop),binary],label+".asm")==0,
                    "IOP disassembly failed")
        (output/"assembly.diff").write_text("".join(difflib.unified_diff(
            (output/"original.asm.log").read_text().splitlines(True),(output/"candidate.asm.log").read_text().splitlines(True),
            fromfile="original",tofile="candidate")))
        module_context_identity = (len(emission["functions"]) == 1 and
                                   not packet["generated_data"] and
                                   not emission["functions"][0]["size_equal"])
        emission["standalone_verified"] = emission["verified"]
        emission["module_context_identity"] = module_context_identity
        require(all(f["compiler_emitted"] and f["binding_equal"] and
                    (f["size_equal"] or module_context_identity) for f in emission["functions"])
                and all(d["verified"] for d in emission["data_objects"])
                and not result["extra_functions"] and not result["extra_sections"],
                "IOP compiler source-emission/identity audit failed")
        from tools import iop_module
        helper=Path(iop_module.__file__).resolve()
        require(helper==(root/"tools/iop_module.py").resolve(),"IOP module verifier belongs to another snapshot")
        inputs[str(helper)]=digest(helper)
        descriptor=dict(object=str(output/"candidate.o"),assembly=str(output/"candidate.s"),
            module_context_identity=module_context_identity,
            functions=[dict(name=f["name"],offset=address(f["va"]),size=f["size"]) for f in packet["functions"]],
            text=dict(offset=address(packet["text"]["va"]),size=packet["text"]["size"]),
            generated_data=[dict(section=s["section"],offset=address(s["va"]),size=s["size"]) for s in packet["generated_data"]])
        module_report=iop_module.build_module(root,packet["unit"],candidate_objects=[descriptor],
                                              build_dir=output/"module-build",output=output/"candidate.irx")
        result["module_reconstruction"]=module_report
        fresh=output/"candidate.irx"
        require(fresh.is_file(),"IOP module verifier produced no fresh IRX: "+str(module_report.get("error","no diagnostic")))
        reconstructed=ELF(fresh.read_bytes())
        whole_equal=fresh.read_bytes()==original_data
        result["module_reconstruction"].update(whole_file_equal=whole_equal,
            original_sha256=digest(original_data),candidate_sha256=digest(fresh))
        data_checks=[item for item in emission["data_objects"] if item["linked_identity_required"]]
        if data_checks:
            linked_path=Path(module_report["linked_elf"]).resolve()
            require(linked_path.is_file() and linked_path.is_relative_to(output/"module-build"),
                    "IOP source-defined data lacks a fresh module-linked identity witness")
            linked_module=ELF(linked_path.read_bytes())
            for item in data_checks:
                symbols=[s for s in linked_module.symbols if s["name"]==item["name"] and s["section_index"]!=0]
                emitted=symbols[0] if len(symbols)==1 else None
                reference=item["original_symbol"]
                item["linked_symbol"]=emitted
                item["linked_identity_equal"]=bool(emitted and reference and emitted["type"]==1
                    and emitted["value"]==reference["value"] and emitted["binding"]==BINDINGS[reference["binding"]]
                    and emitted["other"]==reference["other"] and
                    (not reference["size"] or not emitted["size"] or emitted["size"]==reference["size"]))
                item["verified"] &= item["linked_identity_equal"]
            emission["verified"] &= all(item["verified"] for item in data_checks)
        module_functions=compare_functions(reconstructed)
        final_emission = {f["name"]: f for f in module_report.get("functions", [])}
        for item in emission["functions"]:
            final = final_emission.get(item["name"])
            require(final is not None, "IOP module lacks compiler definition identity witness")
            item["module_linked_symbol"] = final["linked_symbol"]
            item["module_identity_equal"] = all(final["linked_symbol"][key] == final["original_symbol"][key]
                for key in ("name", "value", "size", "type", "binding", "other"))
            item["verified"] = item["compiler_emitted"] and item["binding_equal"] and item["module_identity_equal"]
        emission["verified"] = (all(f["verified"] for f in emission["functions"])
            and all(d["verified"] for d in emission["data_objects"])
            and not result["extra_functions"] and not result["extra_sections"])
        for before,after in zip(result["functions"],module_functions):
            require(before["name"]==after["name"] and
                    (before["actual_sha256"]==after["actual_sha256"] or module_context_identity),
                    "IOP reconstructed body differs from independently compiled/linked C")
        result["standalone_functions"] = result["functions"]
        result["functions"]=module_functions
        scope_records=scoped_relocations(reconstructed,scoped)
        result["relocations_equal"]=(scope_records==original_rels and
            (result["relocation_comparison"]["diagnostic_equal"] or module_context_identity))
        result["spans"]=[]
        for span,before in zip(spans,expected):
            section,after=reconstructed.at(address(span["va"]),span["size"])
            same=before==after
            check=dict(span,**difference(before or b"",after or b"",address(span["va"])),
                       layout_equal=section["name"]==span["section"],verified=same and section["name"]==span["section"])
            result["spans"].append(check)
        module_ok=bool(module_report.get("gate_passed",module_report.get("ok",False)) and whole_equal)
        source_ok=result["source_eligibility"]["verified"] and emission["verified"] and all(
            f["compiler_emitted"] for f in emission["functions"])
        scope_ok=all(s["verified"] for s in result["spans"])
        gate=module_ok and source_ok and scope_ok and result["relocations_equal"] and all(
            f["equal"] and f["binding_equal"] and f["identity_equal"] for f in result["functions"])
        for function in result["functions"]:
            function["compiler_emitted"]=source_ok
            function["verified"]=gate
        result.update(gate_passed=gate,code_bytes_verified=gate,data_generated_checked=scope_ok and module_ok,
                      unit_verified=module_ok)
    except (OSError,ValueError,KeyError,TypeError,ImportError,StopIteration) as exc:
        result.update(error=str(exc),gate_passed=False,code_bytes_verified=False)
    for path,expected_hash in inputs.items():
        if not Path(path).is_file() or digest(path)!=expected_hash:
            result.update(error="IOP input changed during runner: "+path,gate_passed=False,code_bytes_verified=False)
            for function in result["functions"]:
                function["verified"]=False
    (output/"bytes.diff").write_text(json.dumps(result["spans"],indent=2)+"\n")
    result["artifacts"]={str(p.relative_to(output)):digest(p) for p in sorted(output.rglob("*")) if p.is_file()}
    (output/"result.json").write_text(json.dumps(result,indent=2,sort_keys=True)+"\n")
    return result


def main(argv=None):
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root",type=Path,required=True)
    parser.add_argument("--packet",type=Path,required=True)
    parser.add_argument("--output-dir",type=Path,required=True)
    parser.add_argument("--profile",type=Path,default=BASE/"config/iop-match-profile.json")
    args=parser.parse_args(argv)
    try:
        result=match(args.root,args.packet,args.output_dir,args.profile)
        print(json.dumps({k:result[k] for k in ("gate_passed","code_bytes_verified","data_generated_checked",
                          "external_data_witnessed","unit_verified","source_language","recovery_kind")}))
        return 0 if result["gate_passed"] else 1
    except (OSError,ValueError,KeyError,TypeError,StopIteration) as exc:
        print(json.dumps(dict(gate_passed=False,error=str(exc))))
        return 1


if __name__=="__main__":
    sys.exit(main())
