#!/usr/bin/env python3
"""Generate an unmodified native objdiff report after exact linked verification."""
import argparse
import os
from pathlib import Path
import subprocess
import sys

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parent))
from tu import objdiff_inputs, verify_report as verification


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('-o', '--output', default='build/report.json')
    ap.add_argument('--objdiff', type=Path, default=os.environ.get('OBJDIFF_CLI'))
    ap.add_argument('--root', type=Path, default=verification.ROOT)
    args = ap.parse_args()
    root = args.root.resolve()
    output = root / args.output
    output.parent.mkdir(parents=True, exist_ok=True)
    output.unlink(missing_ok=True)
    try:
        verification.require(args.objdiff is not None, 'set OBJDIFF_CLI or pass --objdiff')
        binary = args.objdiff.resolve()
        pin = verification.read(root / 'config/report-tools.json')['objdiff-cli']
        verification.require(verification.digest(binary) == pin['sha256'],
                             'objdiff-cli does not match config/report-tools.json')
        evidence = verification.load_verified(root, root / 'build')
        project, expected = objdiff_inputs.prepare(root, evidence)
        temporary = project / 'report.pending.json'
        temporary.unlink(missing_ok=True)
        subprocess.run(['timeout', '-k', '5', '120', str(binary), 'report', 'generate',
                        '--project', str(project), '--output', str(temporary)],
                       env={**os.environ, 'RAYON_NUM_THREADS': '4'}, check=True)
        objdiff_inputs.validate(verification.read(temporary), expected)
        verification.load_verified(root, root / 'build')
        # Preserve the native report without rewriting scores or counts.
        temporary.replace(output)
        print(f'Native objdiff report: {output}')
        return 0
    except (verification.VerificationError, OSError, subprocess.SubprocessError) as error:
        print(f'objdiff_report: {error}', file=sys.stderr)
        if isinstance(error, subprocess.CalledProcessError) and error.stderr:
            print(error.stderr.decode(errors='replace')[:4000], file=sys.stderr)
        return 1


if __name__ == '__main__':
    sys.exit(main())
