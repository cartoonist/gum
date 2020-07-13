# coding=utf-8

"""
    vg_inspect.py
    ~~~~~~~~~~~~~

    Inspect a vg file.

    :copyright: (c) 2020 by Ali Ghaffaari.
    :license: MIT, see LICENSE for more details.
"""

from collections import deque
import itertools

import stream
import vg_pb2 as vg


def count_chunks(path):
    """Count `vg::Graph` chunks in a vg file."""
    # Counting comes from: https://stackoverflow.com/a/34404546/357257
    cnt = itertools.count()  # Make a stateful counting iterator
    # zip it with the input iterator, then drain until input exhausted at C level
    # cnt must be second zip arg to avoid advancing too far
    deque(zip(stream.parse(path, vg.Graph), cnt), 0)
    # Since count 0 based, the next value is the count
    return next(cnt)
