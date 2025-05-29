import os
from pathlib import Path
import pytest
import subprocess


# Fixture to run test in current tmp_path
@pytest.fixture
def run_in_tmp_path(tmp_path):
    original_cwd = os.getcwd()
    os.chdir(tmp_path)
    yield
    os.chdir(original_cwd)


def test_init_in_directory(tmp_path):
    # tmp_path exists and is empty.
    assert list(tmp_path.iterdir()) == []

    cmd = ['/Users/iant/github/git2cpp/build/git2cpp', 'init', '--bare', str(tmp_path)]
    p = subprocess.run(cmd, capture_output=True)
    assert p.returncode == 0
    assert p.stdout == b''
    assert p.stderr == b''

    assert sorted(map(lambda path: path.name, tmp_path.iterdir())) == [
        'HEAD', 'config', 'description', 'hooks', 'info', 'objects', 'refs'
    ]

    # TODO: check this is a valid git repo


def test_init_in_cwd(tmp_path, run_in_tmp_path):
    # tmp_path exists and is empty.
    assert list(tmp_path.iterdir()) == []
    assert Path.cwd() == tmp_path

    cmd = ['/Users/iant/github/git2cpp/build/git2cpp', 'init', '--bare']
    p = subprocess.run(cmd, capture_output=True)
    assert p.returncode == 0
    assert p.stdout == b''
    assert p.stderr == b''

    assert sorted(map(lambda path: path.name, tmp_path.iterdir())) == [
        'HEAD', 'config', 'description', 'hooks', 'info', 'objects', 'refs'
    ]

    # TODO: check this is a valid git repo


# TODO: Test without bare flag.


def test_error_on_unknown_option():
    cmd = ['build/git2cpp', 'init', '--unknown']
    p = subprocess.run(cmd, capture_output=True)
    assert p.returncode == 1
    assert p.stdout == b''
    assert p.stderr.startswith(b"The following argument was not expected: --unknown")


def test_error_on_repeated_directory():
    cmd = ['build/git2cpp', 'init', 'abc', 'def']
    p = subprocess.run(cmd, capture_output=True)
    assert p.returncode == 1
    assert p.stdout == b''
    assert p.stderr.startswith(b"The following argument was not expected: def")
