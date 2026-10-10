#!/usr/bin/env python3
"""Validate and compose immutable same-TU candidate function dependencies.

This module is shared by the worker, dispatcher and review stage.  Dependencies
are private compiler inputs only: the worker's submitted patch restores their
function blocks to the published base, and only its own allocation receives
new candidate credit.
"""
from __future__ import annotations

import hashlib
import ast
import json
import re
import sqlite3
from pathlib import Path


SCHEMA = 'tu-candidate-dependencies/1'


class DependencyError(ValueError):
    """A candidate dependency is stale, unaudited, or outside its declared scope."""


DEPENDENCY_REQUEST_KEYS = frozenset({
    'task', 'submission_id', 'result_path', 'result_sha256',
    'tu_source_path', 'tu_source_sha256', 'functions', 'tu_id', 'revision',
    'header_path', 'header_sha256', 'form', 'form_source_path', 'form_source_sha256',
})
DEPENDENCY_PACKET_KEYS = frozenset({
    'request', 'result', 'source_text', 'source_path', 'source_sha256',
    'functions', 'form', 'form_source_sha256', 'header_sha256', 'header_text',
    'published_header_text',
})


def _reviewed_declaration_context(db, request):
    """Bind declaration refinement to a current independent ready receipt."""
    try:
        rows = _ro_rows(db, 'SELECT reviewer, report, report_sha256 FROM integration_ready '
                        'WHERE task_id=? AND submission_id=? AND closed_at IS NULL',
                        (request['task'], request['submission_id']))
    except sqlite3.OperationalError as exc:
        if 'no such table: integration_ready' in str(exc):
            return None
        raise
    if not rows:
        return None
    if len(rows) != 1:
        raise DependencyError('declaration refinement has ambiguous readiness')
    row = rows[0]
    report_path = Path(row['report']).resolve()
    if not report_path.is_file() or sha256_file(report_path) != row['report_sha256']:
        raise DependencyError('declaration refinement ready report differs from its queue pin')
    report = read_json(report_path)
    if report.get('reviewer') != row['reviewer']:
        raise DependencyError('declaration refinement reviewer differs from its queue pin')
    matches = []
    for candidate in report.get('candidates') or []:
        if (candidate.get('tu') != request['tu_id'] or
                candidate.get('verdict') != 'ready_to_integrate' or
                candidate.get('review_complete') is not True):
            continue
        for source in candidate.get('sources') or []:
            if (source.get('task') == request['task'] and
                    source.get('submission_id') == request['submission_id'] and
                    (source.get('result') or {}).get('sha256') == request['result_sha256'] and
                    (source.get('submitted_tu') or {}).get('sha256') == request['tu_source_sha256'] and
                    source.get('author') != row['reviewer']):
                matches.append(candidate)
    if len(matches) != 1:
        raise DependencyError('declaration refinement lacks one independently reviewed source identity')
    files = [item for item in matches[0].get('files') or []
             if item.get('role') == 'tu_source' and item.get('action') == 'patch' and
             (item.get('base') or {}).get('origin') == 'published' and
             (item.get('result') or {}).get('sha256') == request['tu_source_sha256']]
    if len(files) != 1:
        raise DependencyError('declaration refinement lacks pinned published-before/provider-after files')
    before, after = files[0]['base'], files[0]['result']
    for image in (before, after):
        if not Path(image['copy']).is_file() or sha256_file(image['copy']) != image['sha256']:
            raise DependencyError('declaration refinement review source copy differs from its pin')
    return dict(schema='reviewed-owned-declarations/1', queue_db=str(Path(db).resolve()),
                report_path=str(report_path), report_sha256=row['report_sha256'],
                reviewer=row['reviewer'], before=before, after=after)


def _nominal_struct(raw, tu_edit):
    code = tu_edit.lexical(raw).strip()
    match = re.fullmatch(r'typedef\s+struct\s+(\w+)\s*\{(.*)\}\s*(\w+)\s*;', code, re.S)
    if not match:
        raise DependencyError('owned refinement requires a named plain struct typedef')
    identity = (match[1], match[3])
    members, offset, alignment = {}, 0, 1
    scalar = {'char': (1, 1), 'signed char': (1, 1), 'unsigned char': (1, 1),
              'short': (2, 2), 'unsigned short': (2, 2),
              'int': (4, 4), 'unsigned int': (4, 4), 'float': (4, 4)}
    for declaration in match[2].split(';'):
        if not declaration.strip():
            continue
        member = re.fullmatch(r'\s*(.*?)\b(\w+)\s*(?:\[\s*(0x[0-9a-fA-F]+|[0-9]+)\s*\])?\s*',
                              declaration, re.S)
        if not member:
            raise DependencyError('owned refinement contains an unsupported member declarator')
        type_text = ' '.join(member[1].split())
        pointer = type_text.endswith('*')
        if pointer:
            if not re.fullmatch(r'(?:struct\s+)?[A-Za-z_]\w*\s*\*', type_text):
                raise DependencyError('owned refinement contains an unsupported pointer type')
            width, align = 4, 4
        elif type_text in scalar:
            width, align = scalar[type_text]
        else:
            raise DependencyError('owned refinement contains an unsupported scalar type')
        count = int(member[3], 0) if member[3] else 1
        if not 0 < count <= 1048576 or member[2] in members:
            raise DependencyError('owned refinement has an invalid or duplicate member')
        offset = (offset + align - 1) // align * align
        members[member[2]] = dict(offset=offset, size=width * count, type=type_text,
                                   pointer=pointer, array=member[3] is not None)
        offset += width * count
        alignment = max(alignment, align)
    extent = (offset + alignment - 1) // alignment * alignment
    return identity, members, extent


def _validate_nominal_refinement(before, after, tu_edit):
    old_identity, old_members, old_extent = _nominal_struct(before, tu_edit)
    new_identity, new_members, new_extent = _nominal_struct(after, tu_edit)
    if old_identity != new_identity or old_extent != new_extent:
        raise DependencyError('owned refinement changes nominal identity or EE extent')
    opaque = [(item['offset'], item['offset'] + item['size'])
              for name, item in old_members.items() if name.startswith('unmodeled_')]
    for name, item in old_members.items():
        if name.startswith('unmodeled_'):
            continue
        replacement = new_members.get(name)
        if (replacement is None or replacement['array'] != item['array'] or
                (replacement['offset'], replacement['size']) != (item['offset'], item['size'])):
            raise DependencyError('owned refinement removes or moves an existing modeled member')
        if replacement['type'] != item['type']:
            if not (item['type'] in ('int', 'unsigned int') and not item['array'] and
                    replacement['pointer'] and not replacement['array']):
                raise DependencyError('owned refinement changes an existing member type')
    for name, item in new_members.items():
        if name in old_members and not name.startswith('unmodeled_'):
            continue
        if not any(start <= item['offset'] and item['offset'] + item['size'] <= end
                   for start, end in opaque):
            raise DependencyError('owned refinement adds a member outside an original opaque span')
    return dict(identity=list(old_identity), extent=old_extent,
                before_sha256=sha256_bytes(before.encode()), after_sha256=sha256_bytes(after.encode()))


def _refine_reviewed_prelude(current, provider, record, tu_edit):
    patch = tu_edit.make_patch(current, provider, names=[block.key for block in current.functions()])
    removals = (patch.get('prelude') or {}).get('remove') or []
    if not removals:
        return current, []
    proof = record.get('reviewed_owned_declarations')
    if (not isinstance(proof, dict) or not isinstance(proof.get('queue_db'), str) or
            proof != _reviewed_declaration_context(proof['queue_db'], record['request'])):
        raise DependencyError('owned prelude refinement is not bound to a current independent review')
    if (sha256_bytes(record['source_text'].encode('utf-8', errors='surrogateescape')) != record['source_sha256'] or
            record['source_sha256'] != proof['after']['sha256']):
        raise DependencyError('owned refinement provider source differs from its reviewed after image')
    reviewed_before = tu_edit.TU(Path(proof['before']['copy']).read_text(), None)
    replacements, evidence = [], []
    old_items = tu_edit.scan(current.prelude())
    provider_items = tu_edit.scan(provider.prelude())
    reviewed_items = tu_edit.scan(reviewed_before.prelude())
    allowed_additions = {row['text'] for row in _provider_prelude_allowlist(record, patch, tu_edit)}
    declared = set().union(*(tu_edit.decl_keys(item.text(current.prelude())) for item in old_items))
    for removed in removals:
        removed_norm = tu_edit._norm(removed)
        keys = tu_edit.decl_keys(removed)
        before = [item for item in old_items if tu_edit._norm(item.text(current.prelude())) == removed_norm]
        prior = [item.text(reviewed_before.prelude()) for item in reviewed_items
                 if tu_edit._norm(item.text(reviewed_before.prelude())) == removed_norm]
        after = [item.text(provider.prelude()) for item in provider_items
                 if item.kind == 'decl' and not item.conditional and tu_edit.decl_keys(item.text(provider.prelude())) == keys]
        if (len(before) != 1 or len(prior) != 1 or len(after) != 1 or
                before[0].kind != 'decl' or before[0].conditional):
            raise DependencyError('owned prelude refinement lacks a unique pinned before/after declaration')
        old_raw = before[0].text(current.prelude())
        if old_raw != prior[0]:
            raise DependencyError('owned refinement original entity differs from its reviewed before image')
        detail = _validate_nominal_refinement(old_raw, after[0], tu_edit)
        # A newly specialized pointer may need an additive typedef before the
        # refined owner. Import only the same declaration the existing provider
        # selector authorizes; never synthesize a forward typedef or type view.
        introduced = []
        _, new_members, _ = _nominal_struct(after[0], tu_edit)
        for member in new_members.values():
            if not member['pointer']:
                continue
            pointee = member['type'].rstrip('*').strip()
            if pointee in ('void', 'int') or pointee.startswith('struct ') or ('ordinary', pointee) in declared:
                continue
            needed = [item.text(provider.prelude()) for item in provider_items
                      if item.kind == 'decl' and not item.conditional and
                      ('ordinary', pointee) in tu_edit.decl_keys(item.text(provider.prelude()))]
            if len(needed) != 1 or needed[0] not in allowed_additions:
                raise DependencyError('owned refinement needs an unauthorized prerequisite declaration')
            _check_declaration_conflicts(current.text, introduced + needed,
                                         record['request']['task'], 'owned refinement', tu_edit)
            introduced.extend(needed)
            declared.update(tu_edit.decl_keys(needed[0]))
        replacement = '\n\n'.join(introduced + after)
        replacements.append((before[0].start, before[0].end, replacement))
        detail['prerequisite_declarations'] = [dict(text=raw, sha256=sha256_bytes(raw.encode()))
                                              for raw in introduced]
        evidence.append(dict(detail, review=proof))
    text = current.text
    for start, end, replacement in sorted(replacements, reverse=True):
        text = text[:start] + replacement + text[end:]
    return tu_edit.TU(text, current.path), evidence


def sha256_bytes(data):
    return hashlib.sha256(data).hexdigest()


def sha256_file(path):
    return sha256_bytes(Path(path).read_bytes())


def read_json(path):
    return json.loads(Path(path).read_text(encoding='utf-8'))


def _ro_rows(db, sql, params=()):
    connection = sqlite3.connect(f'file:{Path(db).resolve()}?mode=ro', uri=True, timeout=10)
    try:
        connection.row_factory = sqlite3.Row
        return connection.execute(sql, params).fetchall()
    finally:
        connection.close()


def _latest_submission(db, task):
    rows = _ro_rows(db, 'SELECT status, artifact FROM tasks WHERE id=?', (task,))
    if not rows:
        raise DependencyError(f'{task}: dependency task is absent from the queue')
    row = rows[0]
    if row['status'] != 'submitted':
        raise DependencyError(f'{task}: dependency task state is {row["status"]!r}, not submitted')
    submits = _ro_rows(db, 'SELECT detail FROM audit WHERE task_id=? AND action=? ORDER BY id',
                       (task, 'submit'))
    events = []
    for item in submits:
        try:
            detail = json.loads(item['detail'] or '{}')
        except ValueError:
            continue
        events.append(detail)
    if not events:
        raise DependencyError(f'{task}: no immutable submit event exists')
    latest = events[-1]
    if latest.get('artifact') != row['artifact']:
        raise DependencyError(f'{task}: current task artifact differs from its latest submit event')
    return latest


def _result_claim(result, function):
    claims = [item for item in result.get('functions') or []
              if function in (item.get('original_names') or [])]
    if len(claims) != 1:
        raise DependencyError(f'{function}: result does not contain exactly one function claim')
    claim = claims[0]
    if claim.get('state') != 'exact_candidate' or claim.get('source_category') != 'exact_c':
        raise DependencyError(f'{function}: dependency result is not exact_c')
    return claim


def _matching_citable_form(result, function):
    for form in result.get('tu', {}).get('citable_forms') or []:
        gate = form.get('gate') or {}
        audit = form.get('audit') or {}
        audited = (audit.get('functions') or {}).get(function) or {}
        if (gate.get('ok') is True and audit.get('ok') is True and
                audited.get('claimed') is True and audited.get('ok') is True and
                audited.get('category') == 'exact_c'):
            return form
    raise DependencyError(f'{function}: no same-form passing whole-file gate and exact_c TU audit')


def _citable_form_source(form, tu):
    """Resolve the exact source that the cited form gated and audited.

    A subset citation points at ``subset/try-NN/<tu>.c``.  Its parent form's
    ``<tu>.c`` is the failed full-TU source and cannot stand in for it, even
    though the gate/audit summaries are nested under that form in the ledger.
    """
    output_dir = Path(form.get('output_dir') or '').resolve()
    stem = Path(tu.get('path') or '').stem
    if not stem or not output_dir.is_dir():
        raise DependencyError('citable form source directory is missing')
    if form.get('kind') != 'subset':
        return output_dir / f'{stem}.c'

    # Recover the exact subset path from the retained charged-form ledger, not
    # from a guessed filename or the result's submitted copy. The result's
    # citable entry must be the same subset gate/audit recorded there.
    attempt_dir = output_dir.parent.parent
    budget_path = attempt_dir / 'budget.json'
    if not budget_path.is_file():
        raise DependencyError('cited subset form budget is missing')
    try:
        forms = read_json(budget_path).get('forms') or []
    except (OSError, ValueError) as exc:
        raise DependencyError(f'cannot read cited subset form budget: {exc}') from exc
    rows = [item for item in forms if item.get('id') == form.get('form')]
    if len(rows) != 1:
        raise DependencyError('cited subset form is not unique in its retained budget')
    subset = ((rows[0].get('tu') or {}).get('subset') or {}).get('submittable')
    if not isinstance(subset, dict):
        raise DependencyError('cited form has no retained exact-subset record')
    if (subset.get('gate') != form.get('gate') or
            subset.get('audit') != form.get('audit') or
            (subset.get('gate') or {}).get('ok') is not True or
            (subset.get('audit') or {}).get('ok') is not True):
        raise DependencyError('cited subset gate/audit differs from the retained same-form record')
    source_path = Path(subset.get('source') or '').resolve()
    try:
        source_path.relative_to(output_dir)
    except ValueError as exc:
        raise DependencyError('cited subset source escapes its form output directory') from exc
    if (source_path.name != f'{stem}.c' or not source_path.is_file() or
            sha256_file(source_path) != subset.get('sha256')):
        raise DependencyError('cited exact-subset source is missing or differs from its retained hash')
    return source_path


def resolve_current(repo, db, task, functions):
    """Resolve a dependency request to the queue's latest immutable submission.

    The returned fields are suitable for a dispatcher packet.  `functions` is
    the exact dependency allowlist requested by the parent, never inferred from
    the candidate's other recovered functions.
    """
    repo = Path(repo).resolve()
    event = _latest_submission(db, task)
    result_path = Path(event['artifact']).resolve()
    result_hash = sha256_file(result_path)
    result = read_json(result_path)
    if result.get('task_id') != task:
        raise DependencyError(f'{task}: immutable result names a different task')
    tu = result.get('tu') or {}
    source = tu.get('submitted') or {}
    source_path = Path(source.get('path') or '').resolve()
    source_hash = source.get('sha256')
    if not source_path.is_file() or sha256_file(source_path) != source_hash:
        raise DependencyError(f'{task}: submitted TU source is missing or differs from result hash')
    allocated = set(tu.get('allocated') or [])
    functions = sorted(set(functions))
    if not functions:
        raise DependencyError(f'{task}: dependency function allowlist is empty')
    source_form = None
    for name in functions:
        if name not in allocated:
            raise DependencyError(f'{task}: {name} was not allocated by this immutable submission')
        _result_claim(result, name)
        form = _matching_citable_form(result, name)
        form_source = _citable_form_source(form, tu)
        if not form_source.is_file() or sha256_file(form_source) != source_hash:
            raise DependencyError(f'{task}: {name} form source does not match the submitted TU hash')
        if source_form is None:
            source_form = (form, form_source)
        elif source_form[0].get('form') != form.get('form'):
            raise DependencyError(f'{task}: dependency functions are not proved by one common exact form')
    header_path = source_path.with_suffix('.h')
    form_header = Path(source_form[0].get('output_dir') or '') / f'{Path(tu.get("path", "")).stem}.h'
    header_exists = header_path.is_file()
    if header_exists != form_header.is_file():
        raise DependencyError(f'{task}: candidate TU header presence differs from the cited form')
    header_hash = sha256_file(header_path) if header_exists else None
    if header_exists and sha256_file(form_header) != header_hash:
        raise DependencyError(f'{task}: cited form header differs from submitted TU header')
    return dict(task=task, submission_id=event.get('submission_id'),
                result_path=str(result_path), result_sha256=result_hash,
                tu_source_path=str(source_path), tu_source_sha256=source_hash,
                functions=functions, tu_id=tu.get('id'),
                revision=(result.get('target') or {}).get('revision'),
                header_path=str(header_path) if header_exists else None,
                header_sha256=header_hash,
                form=source_form[0].get('form'),
                form_source_path=str(source_form[1]),
                form_source_sha256=sha256_file(source_form[1]))


def load_dependencies(repo, db, requests, tu_id, target_functions, revision, tu_edit):
    """Recheck dispatcher pins and return verified records with source text."""
    repo = Path(repo).resolve()
    target_functions = set(target_functions)
    seen = set()
    records = []
    for request in requests or []:
        if not isinstance(request, dict) or set(request) != DEPENDENCY_REQUEST_KEYS:
            raise DependencyError('dependency request does not contain the complete pinned request schema')
        task = request.get('task')
        event = _latest_submission(db, task)
        if event.get('submission_id') != request.get('submission_id'):
            raise DependencyError(f'{task}: latest submission SID changed after dispatch')
        result_path = Path(event['artifact']).resolve()
        if str(result_path) != str(Path(request.get('result_path') or '').resolve()):
            raise DependencyError(f'{task}: queue artifact path differs from pinned dependency path')
        if sha256_file(result_path) != request.get('result_sha256'):
            raise DependencyError(f'{task}: result artifact hash differs from dispatcher pin')
        result = read_json(result_path)
        if result.get('task_id') != task:
            raise DependencyError(f'{task}: pinned immutable result names a different task')
        tu = result.get('tu') or {}
        if tu.get('id') != tu_id or request.get('tu_id') != tu_id:
            raise DependencyError(f'{task}: dependency is not in target TU {tu_id}')
        if (result.get('target') or {}).get('revision') != revision or request.get('revision') != revision:
            raise DependencyError(f'{task}: dependency binary revision differs from target')
        source = tu.get('submitted') or {}
        source_path = Path(source.get('path') or '').resolve()
        if (str(source_path) != str(Path(request.get('tu_source_path') or '').resolve()) or
                source.get('sha256') != request.get('tu_source_sha256') or
                not source_path.is_file() or sha256_file(source_path) != source.get('sha256')):
            raise DependencyError(f'{task}: submitted TU source differs from dispatcher pins')
        header_path = source_path.with_suffix('.h')
        if header_path.is_file():
            if (request.get('header_path') is None or
                    str(header_path) != str(Path(request['header_path']).resolve()) or
                    sha256_file(header_path) != request.get('header_sha256')):
                raise DependencyError(f'{task}: dependency TU header differs from dispatcher pins')
        elif request.get('header_path') is not None or request.get('header_sha256') is not None:
            raise DependencyError(f'{task}: dispatcher pinned a missing dependency header')
        form_name = request.get('form')
        forms = [item for item in tu.get('citable_forms') or [] if item.get('form') == form_name]
        if len(forms) != 1:
            raise DependencyError(f'{task}: pinned citable form {form_name!r} is absent')
        form = forms[0]
        if (form.get('gate') or {}).get('ok') is not True or (form.get('audit') or {}).get('ok') is not True:
            raise DependencyError(f'{task}: pinned form gate or audit is no longer passing')
        form_source = _citable_form_source(
            dict(form, form=form_name), tu)
        if (str(form_source.resolve()) != str(Path(request.get('form_source_path') or '').resolve()) or
                not form_source.is_file() or sha256_file(form_source) != request.get('form_source_sha256') or
                sha256_file(form_source) != source.get('sha256')):
            raise DependencyError(f'{task}: pinned form source hash does not match submitted TU')
        functions = sorted(set(request.get('functions') or []))
        if not functions:
            raise DependencyError(f'{task}: dependency allowlist is empty')
        overlap = sorted(set(functions) & target_functions)
        if overlap:
            raise DependencyError(f'{task}: dependency function overlaps target allocation: {overlap}')
        duplicates = sorted(set(functions) & seen)
        if duplicates:
            raise DependencyError(f'{task}: dependency functions duplicate an earlier dependency: {duplicates}')
        for name in functions:
            _result_claim(result, name)
            audit = (form.get('audit') or {}).get('functions') or {}
            row = audit.get(name) or {}
            if not (row.get('claimed') and row.get('ok') and row.get('category') == 'exact_c'):
                raise DependencyError(f'{task}: {name} is not exact_c in the pinned form audit')
        published_header = repo / str(Path(tu.get('path')).with_suffix('.h'))
        header_text = None
        header_sha256 = None
        published_header_text = None
        if request.get('header_path') is not None:
            candidate_header = Path(request['header_path'])
            header_bytes = candidate_header.read_bytes()
            header_text = header_bytes.decode('utf-8', errors='surrogateescape')
            header_sha256 = sha256_bytes(header_bytes)
            form_header = Path(form.get('output_dir') or '') / f'{Path(tu.get("path")).stem}.h'
            if (not form_header.is_file() or sha256_file(form_header) != header_sha256):
                raise DependencyError(f'{task}: private header differs from the cited exact form')
        if published_header.is_file() and request.get('header_path') is None:
            raise DependencyError(f'{task}: dependency omitted a published TU header')
        if published_header.is_file():
            published_header_text = published_header.read_text(encoding='utf-8', errors='surrogateescape')
        candidate = tu_edit.TU(source_path.read_text(encoding='utf-8', errors='surrogateescape'), str(source_path))
        blocks = {block.key: block for block in candidate.functions()}
        for name in functions:
            block = blocks.get(name)
            if block is None or block.state != 'c':
                raise DependencyError(f'{task}: dependency function {name} is not a C definition')
        seen.update(functions)
        records.append(dict(request=request, result=result, source_text=source_path.read_text(
            encoding='utf-8', errors='surrogateescape'), source_path=str(source_path),
            header_text=header_text, header_sha256=header_sha256,
            published_header_text=published_header_text,
            source_sha256=source.get('sha256'), functions=functions, form=form_name,
            form_source_sha256=request.get('form_source_sha256'),
            reviewed_owned_declarations=_reviewed_declaration_context(db, request)))
    return records


def load_packet_dependencies(repo, db, packets, tu_id, target_functions, revision, tu_edit):
    """Revalidate the dispatcher's complete packet records against current immutable inputs.

    ``module_dispatch`` places verified dependency records in the worker packet,
    rather than flat requests: each row contains the original request pins plus
    the submitted result and C source used to compose the private TU. Do not
    unwrap the request and trust the remaining payload. Re-resolve every request
    from the queue, then compare every carried payload field with that current
    immutable record before returning the freshly loaded records.
    """
    packets = list(packets or [])
    if not packets:
        return []
    requests = []
    for index, packet in enumerate(packets):
        if not isinstance(packet, dict) or set(packet) not in (
                DEPENDENCY_PACKET_KEYS, DEPENDENCY_PACKET_KEYS | {'reviewed_owned_declarations'}):
            raise DependencyError(f'dependency packet row {index} does not contain the complete pinned record schema')
        request = packet.get('request')
        if not isinstance(request, dict) or set(request) != DEPENDENCY_REQUEST_KEYS:
            raise DependencyError(f'dependency packet row {index} does not contain complete request pins')
        requests.append(request)

    records = load_dependencies(repo, db, requests, tu_id, target_functions, revision, tu_edit)
    if len(records) != len(packets):
        raise DependencyError('dependency packet row count changed during current-pin validation')
    for index, (packet, record) in enumerate(zip(packets, records)):
        comparisons = (
            ('request', packet.get('request'), record['request']),
            ('result', packet.get('result'), record['result']),
            ('source_text', packet.get('source_text'), record['source_text']),
            ('source_sha256', packet.get('source_sha256'), record['source_sha256']),
            ('functions', packet.get('functions'), record['functions']),
            ('form', packet.get('form'), record['form']),
            ('form_source_sha256', packet.get('form_source_sha256'), record['form_source_sha256']),
            ('header_sha256', packet.get('header_sha256'), record['header_sha256']),
            ('header_text', packet.get('header_text'), record['header_text']),
            ('published_header_text', packet.get('published_header_text'), record['published_header_text']),
        )
        for field, supplied, verified in comparisons:
            if supplied != verified:
                raise DependencyError(f'dependency packet row {index} {field} differs from its current immutable pin')
        if ('reviewed_owned_declarations' in packet and
                packet['reviewed_owned_declarations'] != record['reviewed_owned_declarations']):
            raise DependencyError(f'dependency packet row {index} declaration review differs from its current pin')
        supplied_path = packet.get('source_path')
        if not isinstance(supplied_path, str) or Path(supplied_path).resolve() != Path(record['source_path']).resolve():
            raise DependencyError(f'dependency packet row {index} source_path differs from its current immutable pin')
    return records


def _interstitial_additions(base_text, candidate_text, task, function, function_code, tu_edit):
    """Return a candidate interstitial only when it adds declarations safely.

    Function-scoped candidate imports may need declarations placed between two
    TU functions (rather than in the file prelude).  Treat that region as an
    additive patch: every existing item must remain byte-equivalent after
    normalization, and each new top-level item must be an unconditional C
    declaration. Comments, directives, scaffolding, and function definitions
    are deliberately not imported through this path. A single-line,
    object-like macro is also eligible when its replacement is a pure integer
    constant expression and the requested body uses its name.
    """
    base_items = tu_edit.scan(base_text)
    candidate_items = tu_edit.scan(candidate_text)
    base_norms = [tu_edit._norm(item.text(base_text)) for item in base_items]
    candidate_norms = [tu_edit._norm(item.text(candidate_text)) for item in candidate_items]

    # Preserve the existing item sequence exactly; additions may be interleaved
    # with declarations but may not replace, reorder, or remove any prior item.
    cursor = 0
    for norm in base_norms:
        try:
            cursor = candidate_norms.index(norm, cursor) + 1
        except ValueError as exc:
            raise DependencyError(
                f'{task}: dependency changes existing interstitial before {function}') from exc

    base_counts = {}
    for norm in base_norms:
        base_counts[norm] = base_counts.get(norm, 0) + 1
    candidate_counts = {}
    additions = []
    for item in candidate_items:
        raw = item.text(candidate_text)
        norm = tu_edit._norm(raw)
        candidate_counts[norm] = candidate_counts.get(norm, 0) + 1
        if candidate_counts[norm] > base_counts.get(norm, 0):
            if item.kind == 'decl' and not item.conditional:
                additions.append(raw)
            elif (item.kind in ('other', 'pp') and not item.conditional and
                  _pure_integer_object_macro(raw, candidate_text, item)):
                name = _pure_integer_object_macro(raw, candidate_text, item)[0]
                if re.search(r'(?<![\w$])' + re.escape(name) +
                             r'(?![\w$])', function_code):
                    additions.append(raw)

    if not additions:
        return None, []
    imported = base_text
    if imported and not imported.endswith('\n'):
        imported += '\n'
    imported += '\n'.join(additions)
    if not imported.endswith('\n'):
        imported += '\n'
    return imported, additions


def _pure_integer_object_macro(raw, source_text, item=None):
    """Return (name,value) only for a one-line side-effect-free integer macro."""
    match = re.fullmatch(r'\s*#\s*define\s+([A-Za-z_]\w*)[ \t]+([^\\\n]+)\s*', raw)
    if not match:
        return None
    name, expression = match.groups()
    # C integer suffixes are not Python syntax. Strip only suffixes attached
    # directly to integer tokens; identifiers and all other operators remain
    # subject to the closed AST whitelist below.
    expression = re.sub(r'(?i)(0[xX][0-9a-f]+|0[bB][01]+|[0-9]+)[uUlL]+\b', r'\1', expression)
    try:
        tree = ast.parse(expression, mode='eval')
    except SyntaxError:
        return None
    allowed_nodes = (ast.Expression, ast.Constant, ast.UnaryOp, ast.UAdd, ast.USub,
                     ast.Invert, ast.BinOp, ast.Add, ast.Sub, ast.Mult, ast.FloorDiv,
                     ast.Mod, ast.LShift, ast.RShift, ast.BitOr, ast.BitAnd, ast.BitXor)
    if any(not isinstance(node, allowed_nodes) for node in ast.walk(tree)):
        return None
    if any(isinstance(node, ast.Constant) and
           (not isinstance(node.value, int) or isinstance(node.value, bool))
           for node in ast.walk(tree)):
        return None
    return name, expression


def _declared_names(text, tu_edit):
    """Use the TU editor's C-aware declaration keys for conflict checks."""
    return tu_edit.decl_keys(text)


def _check_declaration_conflicts(current_text, additions, task, function, tu_edit):
    """Reject imported declarations that collide with different TU declarations."""
    existing = {}
    try:
        items = tu_edit.scan(current_text)
    except Exception as exc:
        raise DependencyError(f'{task}: cannot inspect composed declarations: {exc}') from exc
    for item in items:
        if item.kind != 'decl':
            continue
        raw = item.text(current_text)
        for name in _declared_names(raw, tu_edit):
            existing.setdefault(name, set()).add(tu_edit._norm(raw))

    seen_additions = {}
    for raw in additions:
        macro = _pure_integer_object_macro(raw, raw)
        if macro:
            name, expression = macro
            prior = []
            for item in tu_edit.scan(current_text):
                existing_macro = _pure_integer_object_macro(item.text(current_text), current_text)
                if existing_macro and existing_macro[0] == name:
                    prior.append(existing_macro[1])
            if prior and any(value != expression for value in prior):
                raise DependencyError(
                    f'{task}: imported integer macro {name} conflicts before {function}')
            previous = seen_additions.setdefault(('macro', name), set())
            if previous and expression not in previous:
                raise DependencyError(f'{task}: imported integer macro {name} conflicts before {function}')
            previous.add(expression)
            continue
        normalized = tu_edit._norm(raw)
        names = _declared_names(raw, tu_edit)
        if not names:
            raise DependencyError(
                f'{task}: cannot identify imported declaration before {function}: {raw.strip()[:80]}')
        for name in names:
            prior = existing.get(name, set()) | seen_additions.get(name, set())
            if prior and normalized not in prior:
                raise DependencyError(
                    f'{task}: imported declaration for {name[1]} conflicts with an existing TU declaration')
            seen_additions.setdefault(name, set()).add(normalized)


def _provider_prelude_allowlist(record, patch, tu_edit):
    """Select only added declarations referenced by requested provider C bodies."""
    provider = tu_edit.TU(record['source_text'], record.get('source_path'))
    blocks = {block.key: block for block in provider.functions()}
    requested = set(record['functions'])
    lexical_by_function = {}
    for name in requested:
        block = blocks.get(name)
        if block is None or block.state != 'c':
            raise DependencyError(
                f'{record["request"]["task"]}: provider {name} has no immutable C body for declaration use')
        lexical_by_function[name] = tu_edit.lexical(
            record['source_text'][block.start:block.end])

    allowlist = []
    seen = set()
    for addition in (patch.get('prelude') or {}).get('add') or []:
        raw = addition.get('text') if isinstance(addition, dict) else None
        if not isinstance(raw, str):
            continue
        items = tu_edit.scan(raw)
        if len(items) != 1 or items[0].start != 0 or items[0].end != len(raw):
            continue
        item = items[0]
        if item.kind != 'decl' or item.conditional:
            continue
        keys = sorted(tu_edit.decl_keys(raw))
        if not keys:
            continue
        used = False
        for kind, name in keys:
            if kind == 'function' and name in requested:
                continue
            if any(re.search(r'(?<![\w$])' + re.escape(name) + r'(?![\w$])', code)
                   for code in lexical_by_function.values()):
                used = True
                break
        if not used or raw in seen:
            continue
        seen.add(raw)
        allowlist.append(dict(text=raw, sha256=tu_edit.sha256_text(raw),
                              declared_keys=[list(key) for key in keys]))
    if record.get('header_text') is not None:
        header_rows = _provider_header_allowlist(record, lexical_by_function.values(), tu_edit)
        allowlist = header_rows + allowlist
    unique = {}
    for row in allowlist:
        prior = unique.get(row['text'])
        if prior and prior['sha256'] != row['sha256']:
            raise DependencyError(f'{record["request"]["task"]}: duplicate dependency declaration hash conflict')
        unique[row['text']] = row
    return list(unique.values())


def _provider_header_allowlist(record, function_codes, tu_edit):
    """Select exact header declarations/macros needed by the requested C bodies.

    The candidate header must already be pinned by the submitted result and
    the cited exact form. Its published-header items must remain an ordered
    subset; this path imports only additive declarations used by the selected
    bodies (plus recursively referenced declared types).
    """
    task = record['request']['task']
    candidate = record['header_text']
    published = record.get('published_header_text') or ''
    def include_guard_body(text):
        match = re.match(r'\s*#ifndef\s+([A-Za-z_]\w*)\s*\n\s*#define\s+\1\s*(?:\n|$)', text)
        if not match:
            return text
        tail = re.search(r'(?m)^#endif\b[^\n]*\s*$', text[match.end():])
        if not tail:
            raise DependencyError(f'{task}: private header has an unterminated include guard')
        return text[match.end():match.end() + tail.start()]
    candidate_body = include_guard_body(candidate)
    published_body = include_guard_body(published) if published else ''
    candidate_items = tu_edit.scan(candidate_body)
    base_items = tu_edit.scan(published_body) if published_body else []
    base_decls = [tu_edit._norm(item.text(published_body)) for item in base_items if item.kind == 'decl']
    candidate_decls = [tu_edit._norm(item.text(candidate_body)) for item in candidate_items if item.kind == 'decl']
    cursor = 0
    for raw in base_decls:
        try:
            cursor = candidate_decls.index(raw, cursor) + 1
        except ValueError as exc:
            raise DependencyError(f'{task}: cited private header removes or changes a published declaration') from exc

    existing_by_key = {}
    for item in base_items:
        if item.kind == 'decl':
            raw = item.text(published_body)
            for key in _declared_names(raw, tu_edit):
                existing_by_key.setdefault(key, set()).add(tu_edit._norm(raw))

    needed_names = set()
    for code in function_codes:
        needed_names.update(re.findall(r'(?<![\w$])[A-Za-z_]\w*(?![\w$])', code))
    rows = []
    for item in candidate_items:
        raw = item.text(candidate_body)
        if item.kind == 'decl' and not item.conditional:
            keys = sorted(_declared_names(raw, tu_edit))
            if keys:
                rows.append(dict(text=raw, kind='declaration', keys=keys, item=item))
        elif item.kind in ('pp', 'other') and not item.conditional:
            macro = _pure_integer_object_macro(raw, candidate, item)
            if macro:
                rows.append(dict(text=raw, kind='constant_macro', keys=[('macro', macro[0])],
                                 item=item, macro=macro))

    selected, selected_keys, changed = [], set(), True
    while changed:
        changed = False
        for row in rows:
            if id(row) in selected:
                continue
            if not any(key[1] in needed_names for key in row['keys']):
                continue
            selected.append(id(row))
            selected_keys.update(row['keys'])
            needed_names.update(re.findall(r'(?<![\w$])[A-Za-z_]\w*(?![\w$])', row['text']))
            changed = True

    output = []
    for row in rows:
        if id(row) not in selected:
            continue
        raw = row['text']
        keys = row['keys']
        if row['kind'] == 'declaration':
            normalized = tu_edit._norm(raw)
            for key in keys:
                prior = existing_by_key.get(key, set())
                if prior and normalized not in prior:
                    raise DependencyError(f'{task}: private header declaration for {key[1]} conflicts with published owner')
            if all(tu_edit._norm(raw) in existing_by_key.get(key, set()) for key in keys):
                continue
            for key in keys:
                existing_by_key.setdefault(key, set()).add(normalized)
        else:
            name, expression = row['macro']
            existing_macros = []
            for item in base_items:
                macro = _pure_integer_object_macro(item.text(published_body), published_body, item)
                if macro and macro[0] == name:
                    existing_macros.append(macro[1])
            if existing_macros and any(value != expression for value in existing_macros):
                raise DependencyError(f'{task}: private header integer macro {name} conflicts with published header')
            if expression in existing_macros:
                continue
        output.append(dict(text=raw, sha256=sha256_bytes(raw.encode('utf-8', errors='surrogateescape')),
                           declared_keys=[list(key) for key in keys], source='private-header',
                           source_header_sha256=record['header_sha256'], kind=row['kind']))
    return output


def dependency_usage_context(records, tu_edit):
    """Return only the immutable requested exact-C dependency function blocks.

    This text is for unused-declaration accounting only. It must never replace
    the target-only merge input or participate in target function credit.
    """
    snippets = []
    for record in records:
        provider = tu_edit.TU(record['source_text'], record.get('source_path'))
        blocks = {block.key: block for block in provider.functions()}
        for name in record['functions']:
            block = blocks.get(name)
            if block is None or block.state != 'c':
                raise DependencyError(
                    f'{record["request"]["task"]}: dependency-use context {name} is not an exact C body')
            snippets.append(record['source_text'][block.start:block.end])
    return '\n'.join(snippets)


def compose(base_text, records, tu_edit):
    """Apply only verified dependency function blocks to a published-base copy."""
    current = tu_edit.TU(base_text, None)
    base_sha256 = sha256_bytes(base_text.encode('utf-8', errors='surrogateescape'))
    details = []
    for record in records:
        theirs = tu_edit.TU(record['source_text'], record.get('source_path'))
        current, refinements = _refine_reviewed_prelude(current, theirs, record, tu_edit)
        names = [block.key for block in current.functions()]
        base_regions = tu_edit.Regions(current, names, 'dependency base')
        candidate_regions = tu_edit.Regions(theirs, names, 'dependency candidate')
        patch = tu_edit.make_patch(current, theirs, names=names)
        if patch.get('prelude', {}).get('remove'):
            raise DependencyError(f'{record["request"]["task"]}: dependency removes published prelude declarations')
        allowlist = _provider_prelude_allowlist(record, patch, tu_edit)
        try:
            # The low-level selector intentionally rejects non-declaration
            # additions. Give it a synthetic provider containing the exact
            # allowlisted declaration additions only; original provider bytes
            # remain pinned in `record` and in the final manifest. Re-anchor
            # these selected additions to the current base prelude: their
            # original `after` anchors can name an unallowlisted provider item.
            allowlisted_text = {row['text'] for row in allowlist}
            selector_patch = dict(patch)
            selector_patch['functions'] = {}
            selector_patch['interstitial'] = {}
            base_prelude, _ = tu_edit.prelude_items(current.prelude())
            anchor = tu_edit._norm(base_prelude[-1][1]) if base_prelude else None
            original_entries = {item.get('text'): item
                                for item in (patch.get('prelude', {}).get('add') or [])}
            selected_entries = []
            for row in allowlist:
                raw = row['text']
                if raw not in allowlisted_text:
                    continue
                entry = dict(original_entries.get(raw) or
                             dict(text=raw, gap='\n\n'), after=anchor)
                selected_entries.append(entry)
                anchor = tu_edit._norm(raw)
            selector_patch['prelude'] = dict(
                add=selected_entries,
                remove=[])
            selector_patch.pop('epilogue', None)
            selector_provider = tu_edit.patch_to_theirs(current, selector_patch, names=names)
            selector_allowlist = []
            for row in allowlist:
                selected_row = dict(text=row['text'], sha256=row['sha256'],
                                    declared_keys=row['declared_keys'])
                if row.get('kind') == 'constant_macro':
                    selected_row['kind'] = 'constant_macro'
                selector_allowlist.append(selected_row)
            selected = tu_edit.select_dependency_prelude_additions(
                current.text, selector_provider.text, selector_allowlist,
                base_sha256=sha256_bytes(current.text.encode('utf-8', errors='surrogateescape')),
                provider_sha256=tu_edit.sha256_text(selector_provider.text))
        except Exception as exc:
            raise DependencyError(
                f'{record["request"]["task"]}: cannot select exact provider prelude declarations: {exc}') from exc
        selected_prelude = selected['manifest']['additions']
        selected_text = {row['text'] for row in selected_prelude if row.get('status') == 'added'}
        base_prelude, _ = tu_edit.prelude_items(current.prelude())
        anchor = tu_edit._norm(base_prelude[-1][1]) if base_prelude else None
        original_entries = {item.get('text'): item
                            for item in (patch.get('prelude', {}).get('add') or [])}
        anchored_additions = []
        for row in allowlist:
            raw = row['text']
            if raw not in selected_text:
                continue
            entry = dict(original_entries.get(raw) or dict(text=raw, gap='\n\n'), after=anchor)
            anchored_additions.append(entry)
            anchor = tu_edit._norm(raw)
        patch['prelude']['add'] = anchored_additions
        patch['prelude']['remove'] = []
        # The immutable candidate can carry other exact functions. Only the
        # explicitly requested blocks are imported from it.
        patch['functions'] = {name: value for name, value in patch.get('functions', {}).items()
                              if name in record['functions']}
        # Import only additive declaration context immediately before each
        # requested provider function. Other interstitial changes remain out of
        # scope, and the whole-TU compiler gate still validates the result.
        imported_declarations = []
        allowed_interstitial = {}
        for name in record['functions']:
            if name not in base_regions.pre or name not in candidate_regions.pre:
                raise DependencyError(f'{record["request"]["task"]}: missing interstitial for {name}')
            provider_block = next((block for block in theirs.functions() if block.key == name), None)
            if provider_block is None or provider_block.state != 'c':
                raise DependencyError(f'{record["request"]["task"]}: missing immutable C body for {name}')
            function_code = tu_edit.lexical(record['source_text'][provider_block.start:provider_block.end])
            candidate_text, additions = _interstitial_additions(
                base_regions.pre[name], candidate_regions.pre[name],
                record['request']['task'], name, function_code, tu_edit)
            if candidate_text is not None:
                _check_declaration_conflicts(current.text, additions,
                                             record['request']['task'], name, tu_edit)
                allowed_interstitial[name] = candidate_text
                imported_declarations.extend(dict(before_function=name,
                                                  kind=('constant_macro' if _pure_integer_object_macro(item,
                                                        candidate_regions.pre[name]) else 'declaration'),
                                                  sha256=sha256_bytes(item.encode('utf-8', errors='surrogateescape')),
                                                  text=item)
                                             for item in additions)
        patch['interstitial'] = allowed_interstitial
        patch.pop('epilogue', None)
        try:
            current = tu_edit.patch_to_theirs(current, patch, names=names)
        except Exception as exc:
            raise DependencyError(f'{record["request"]["task"]}: cannot compose function patch: {exc}') from exc
        details.append(dict(task=record['request']['task'], submission_id=record['request']['submission_id'],
                            result_sha256=record['request']['result_sha256'],
                            tu_source_sha256=record['source_sha256'],
                            functions=record['functions'], form=record['form'],
                            declarations=imported_declarations,
                            prelude_declarations=selected_prelude,
                            owned_declaration_refinements=refinements))
    manifest = dict(schema=SCHEMA, tu=records[0]['request']['tu_id'] if records else None,
                    base_sha256=base_sha256, dependencies=details,
                    composed_base_sha256=sha256_bytes(current.text.encode('utf-8', errors='surrogateescape')))
    canonical = json.dumps(manifest, sort_keys=True, separators=(',', ':')).encode()
    manifest['manifest_sha256'] = sha256_bytes(canonical)
    return current.text, manifest


def verify_dependencies_present(current_text, composed_base_text, records, tu_edit):
    """Ensure a worker edited only its allocation, not imported dependency bodies."""
    current = tu_edit.TU(current_text, None)
    composed = tu_edit.TU(composed_base_text, None)
    current_blocks = {block.key: (current.text[block.start:block.end], block.state)
                      for block in current.functions()}
    composed_blocks = {block.key: (composed.text[block.start:block.end], block.state)
                       for block in composed.functions()}
    for record in records:
        for name in record['functions']:
            current_block = current_blocks.get(name)
            expected_block = composed_blocks.get(name)
            if (current_block is None or expected_block is None or
                    current_block != expected_block or current_block[1] != 'c'):
                raise DependencyError(f'{record["request"]["task"]}: dependency body {name} changed '
                                      'after private composition')


def verify_dependencies_unmodified(current_text, composed_base_text, published_base_text,
                                   records, tu_edit):
    """At result time accept an intact import or its already projected base form."""
    current = tu_edit.TU(current_text, None)
    composed = tu_edit.TU(composed_base_text, None)
    base = tu_edit.TU(published_base_text, None)
    blocks = lambda tu: {b.key: (tu.text[b.start:b.end], b.state) for b in tu.functions()}
    current_blocks, composed_blocks, base_blocks = blocks(current), blocks(composed), blocks(base)
    for record in records:
        for name in record['functions']:
            actual = current_blocks.get(name)
            allowed = {composed_blocks.get(name), base_blocks.get(name)}
            if actual not in allowed:
                raise DependencyError(f'{record["request"]["task"]}: dependency body {name} '
                                      'changed outside its pinned or projected form')


def target_only(composed_text, base_text, records, tu_edit):
    """Restore dependency function blocks while retaining their additive prelude."""
    names = sorted({name for record in records for name in record['functions']})
    if not names:
        return composed_text
    composed = tu_edit.TU(composed_text, None)
    base = tu_edit.TU(base_text, None)
    all_names = [block.key for block in composed.functions()]
    patch = tu_edit.make_patch(composed, base, names=all_names)
    patch['functions'] = {name: value for name, value in patch.get('functions', {}).items()
                          if name in names}
    patch['interstitial'] = {}
    # Keep dependency declarations in the prelude: the review stage will merge
    # the dependency submission itself, and target compilation used these same
    # declarations. Only unallocated function blocks are projected away.
    patch['prelude'] = dict(add=[], remove=[])
    patch.pop('epilogue', None)
    try:
        return tu_edit.patch_to_theirs(composed, patch, names=all_names).text
    except Exception as exc:
        raise DependencyError(f'cannot project target-only TU patch: {exc}') from exc


def verify_projection(composed_text, target_text, base_text, records, tu_edit):
    """Fail closed unless target text is precisely composed text minus dependency bodies."""
    expected = target_only(composed_text, base_text, records, tu_edit)
    if expected != target_text:
        raise DependencyError('target-only source does not replay from the pinned dependency composition')
    return sha256_bytes(expected.encode('utf-8', errors='surrogateescape'))


def strip_verified_dependency_interstitials(submitted_text, base_text, records, tu_edit):
    """Build an ephemeral target-only merge input without dependency context.

    The handed-off TU remains intact.  This view restores only the exact
    interstitial regions that ``compose`` imported before dependency-owned
    functions, allowing ordinary allocation checks to merge the target patch.
    The dependency-first stage separately contributes those declarations and
    ``verify_composition`` checks that they survived unchanged.
    """
    composed_text, manifest = compose(base_text, records, tu_edit)
    submitted = tu_edit.TU(submitted_text, None)
    base = tu_edit.TU(base_text, None)
    composed = tu_edit.TU(composed_text, None)
    prelude_rows = [row for dependency in manifest['dependencies']
                    for row in dependency.get('prelude_declarations', [])
                    if row.get('status') == 'added']
    remove_spans = []
    source_items = tu_edit.scan(submitted_text)
    for row in prelude_rows:
        raw = row.get('text')
        if not isinstance(raw, str) or tu_edit.sha256_text(raw) != row.get('sha256'):
            raise DependencyError('dependency prelude declaration hash differs from its manifest')
        expected_kind = 'pp' if row.get('kind') == 'constant_macro' else 'decl'
        matches = [item for item in source_items
                   if item.kind == expected_kind and not item.conditional and
                   item.end <= submitted.prelude_end() and item.text(submitted_text) == raw]
        if len(matches) != 1:
            raise DependencyError('target source does not contain one exact dependency prelude declaration or macro')
        remove_spans.append((matches[0].start, matches[0].end))
    ephemeral_text = submitted_text
    for start, end in sorted(remove_spans, reverse=True):
        ephemeral_text = ephemeral_text[:start] + ephemeral_text[end:]
    if remove_spans:
        submitted = tu_edit.TU(ephemeral_text, None)
    names = [block.key for block in submitted.functions()]
    base_regions = tu_edit.Regions(base, names, 'dependency published base')
    composed_regions = tu_edit.Regions(composed, names, 'dependency composed base')
    submitted_regions = tu_edit.Regions(submitted, names, 'dependency target source')
    imported = {row['before_function'] for dependency in manifest['dependencies']
                for row in dependency.get('declarations', [])}
    if not imported:
        return ephemeral_text

    replacements = {}
    for name in sorted(imported):
        if name not in base_regions.pre or name not in composed_regions.pre or name not in submitted_regions.pre:
            raise DependencyError(f'interstitial declaration owner {name} is absent from one TU view')
        expected = composed_regions.pre[name]
        if submitted_regions.pre[name] != expected:
            raise DependencyError(f'target source changed dependency declaration context before {name}')
        replacements[name] = base_regions.pre[name]

    patch = tu_edit.make_patch(submitted, base, names=names)
    patch['functions'] = {}
    patch['prelude'] = dict(add=[], remove=[])
    patch['interstitial'] = replacements
    patch.pop('epilogue', None)
    try:
        projected = tu_edit.patch_to_theirs(submitted, patch, names=names)
    except Exception as exc:
        raise DependencyError(f'cannot construct dependency-free target merge view: {exc}') from exc

    # Prove that the ephemeral view changed only the pinned dependency-owned
    # interstitials.  No function body, prelude or tail text may move here.
    projected_blocks = {block.key: (projected.text[block.start:block.end], block.state)
                         for block in projected.functions()}
    submitted_blocks = {block.key: (submitted.text[block.start:block.end], block.state)
                        for block in submitted.functions()}
    if projected_blocks != submitted_blocks:
        raise DependencyError('dependency-free merge view changed a function block')
    if tu_edit.Regions(projected, names, 'dependency-free target view').prelude != submitted_regions.prelude:
        raise DependencyError('dependency-free merge view changed the target prelude')
    return projected.text


def verify_dependency_bodies(staged_text, composed_base_text, records, tu_edit):
    """Verify the final staged TU retained each pinned dependency body."""
    verify_dependencies_present(staged_text, composed_base_text, records, tu_edit)


def verify_composition(staged_text, composed_base_text, records, tu_edit):
    """Review-stage API for checking dependency bodies in its merged TU."""
    return verify_dependency_bodies(staged_text, composed_base_text, records, tu_edit)
