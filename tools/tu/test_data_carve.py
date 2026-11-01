#!/usr/bin/env python3
"""Unit tests for tools/tu/data_carve.py (standard library only).

    python3 -B -m unittest tools/tu/test_data_carve.py

They exercise pure registry/item logic on synthetic splat data files; the
whole-file gate and tools/tu_audit.py remain the only verdict on a build.
"""
import sys
import tempfile
import unittest
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import data_carve  # noqa: E402

# main/tu206 colli_test .lit4, as splat writes it: CheckBallBoxCollision's
# 0.001f leads the pool, InitTest owns the next eight, ColliTest the last.
LIT4 = [(0x004D8268, '6F12833A'), (0x004D826C, 'E4CB9640'), (0x004D8270, '9A99D93F'),
        (0x004D8274, 'CDCC1CC0'), (0x004D8278, 'CDCC0C40'), (0x004D827C, 'CDCC4CC0'),
        (0x004D8280, 'E17AB4BF'), (0x004D8284, 'DF4FCDBE'), (0x004D8288, 'CDCC4C3F'),
        (0x004D828C, 'CDCCCC3D')]
BALL = {'D_004D8268', 'jtbl_004CBC30'}
INIT = {f'D_{va:08X}' for va, _ in LIT4[1:9]}
COLLI = {'D_004D828C', 'collisionHitLabel'}


def data_file(path, section, rows):
    lines = ['.include "macro.inc"', '', f'.section {section}, "wa"', '']
    for va, word in rows:
        name = f'D_{va:08X}'
        lines += [f'nonmatching {name}', '', f'dlabel {name}',
                  f'    /* {va - 0x1FF000:06X} {va:08X} {word} */ .word 0x{word}',
                  f'enddlabel {name}', f'.size {name}, . - {name}', '']
    Path(path).write_text('\n'.join(lines))


def candidate_run(lo, hi, owners, generated_by):
    return dict(range=[data_carve.h8(lo), data_carve.h8(hi)],
                generated=[data_carve.h8(lo), data_carve.h8(hi)],
                generated_by=sorted(generated_by), c_owned_symbols=sorted(owners),
                also_referenced_by_c=[], basis=dict(kind='candidate', task='test'))


class SubsetLiteralSlotTest(unittest.TestCase):
    """An asm-only literal slot returns to scaffold; C literals keep their addresses."""

    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        path = Path(self.tmp.name) / 'colli_test.lit4.s'
        data_file(path, '.lit4', LIT4)
        piece = dict(start=0x004D8268, end=0x004D8290)
        self.ctx = dict(pieces={'.lit4': [piece]}, path_of=lambda p, sec: path)
        # The whole candidate (CheckBallBoxCollision in C) derived one run.
        self.declared = {'.lit4': [candidate_run(
            0x004D8268, 0x004D8290, {f'D_{va:08X}' for va, _ in LIT4},
            ['CheckBallBoxCollision', 'ColliTest', 'InitTest_002DBE88'])]}
        self.per_func = {'InitTest_002DBE88': INIT, 'ColliTest': COLLI}

    def tearDown(self):
        self.tmp.cleanup()

    def trim(self, declared, scaffold, published=None, per_func=None):
        per_func = self.per_func if per_func is None else per_func
        c_refs = set().union(*per_func.values()) if per_func else set()
        return data_carve.trim_scaffold_only_items(
            self.ctx, declared, c_refs, set(), scaffold, per_func,
            accepted_runs=published or {}, tu_id='main/tu206')

    def test_asm_only_leading_literal_returns_to_scaffold(self):
        trimmed, removed = self.trim(self.declared, {'CheckBallBoxCollision': BALL})
        runs = trimmed['.lit4']
        self.assertEqual(len(runs), 1)
        run = runs[0]
        # InitTest's eight literals and ColliTest's one keep their original
        # addresses: the run now starts at 0x004d826c, not 0x004d8268.
        self.assertEqual(run['range'], ['0x004d826c', '0x004d8290'])
        self.assertEqual(run['generated'], ['0x004d826c', '0x004d8290'])
        self.assertNotIn('D_004D8268', run['c_owned_symbols'])
        self.assertEqual(set(run['c_owned_symbols']), INIT | {'D_004D828C'})
        self.assertEqual(run['generated_by'], ['ColliTest', 'InitTest_002DBE88'])
        self.assertNotIn('c_input_spans', run)
        self.assertEqual([(r['name'], r['range'], r['referenced_by']) for r in removed],
                         [('D_004D8268', ['0x004d8268', '0x004d826c'], ['CheckBallBoxCollision'])])

    def test_asm_only_middle_literal_splits_the_run(self):
        # A scaffold function owning a literal between two C functions' literals.
        per_func = {'InitTest_002DBE88': INIT - {'D_004D8278'},
                    'ColliTest': COLLI | {'D_004D8268'}}
        trimmed, removed = self.trim(self.declared, {'Other': {'D_004D8278'}}, per_func=per_func)
        self.assertEqual([r['range'] for r in trimmed['.lit4']],
                         [['0x004d8268', '0x004d8278'], ['0x004d827c', '0x004d8290']])
        self.assertEqual([r['name'] for r in removed], ['D_004D8278'])

    def test_shared_literal_follows_the_c_function(self):
        # The scaffold function also names an InitTest literal: it stays C.
        trimmed, removed = self.trim(
            self.declared, {'CheckBallBoxCollision': BALL | {'D_004D826C'}})
        self.assertEqual(trimmed['.lit4'][0]['range'], ['0x004d826c', '0x004d8290'])
        self.assertIn('D_004D826C', trimmed['.lit4'][0]['c_owned_symbols'])
        self.assertEqual([r['name'] for r in removed], ['D_004D8268'])

    def test_whole_candidate_keeps_its_run(self):
        # CheckBallBoxCollision is C again: nothing is asm-only.
        per_func = dict(self.per_func, CheckBallBoxCollision=BALL)
        trimmed, removed = self.trim(self.declared, {}, per_func=per_func)
        self.assertEqual(trimmed, self.declared)
        self.assertEqual(removed, [])

    def test_published_runs_are_never_trimmed(self):
        published = {'.lit4': [dict(range=['0x004d8268', '0x004d826c'])]}
        trimmed, removed = self.trim(self.declared, {'CheckBallBoxCollision': BALL},
                                     published=published)
        self.assertEqual(trimmed['.lit4'][0]['range'], ['0x004d8268', '0x004d8290'])
        self.assertEqual(removed, [])
        accepted = dict(self.declared['.lit4'][0], basis=dict(kind='batch07-fresh-derive'))
        trimmed, removed = self.trim({'.lit4': [accepted]}, {'CheckBallBoxCollision': BALL})
        self.assertEqual(trimmed['.lit4'], [accepted])
        self.assertEqual(removed, [])

    def test_unreferenced_item_is_not_inferred_asm_only(self):
        # Referenced by nobody: no evidence it is scaffold-only, keep the request.
        trimmed, removed = self.trim(self.declared, {})
        self.assertEqual(trimmed, self.declared)
        self.assertEqual(removed, [])


class SidecarMessageTest(unittest.TestCase):
    def test_missing_sidecar_names_supported_invocation(self):
        text = data_carve.missing_sidecar(Path('/x/form-01/compiler-emission.o'), '.i')
        self.assertIn('compiler-emission.o.i', text)
        self.assertIn('build/c/<tu>.o', text)
        self.assertIn('--source', text)


if __name__ == '__main__':
    unittest.main()
