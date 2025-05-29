import pytest
import subprocess


@pytest.mark.parametrize("arg", ['-v', '--version'])
def test_version(arg):
    cmd = ['build/git2cpp', arg]
    p = subprocess.run(cmd, capture_output=True)
    assert p.returncode == 0
    assert p.stderr == b''
    assert p.stdout.startswith(b'git2cpp ')


def test_error_on_unknown_option():
    cmd = ['build/git2cpp', '--unknown']
    p = subprocess.run(cmd, capture_output=True)
    assert p.returncode == 1
    assert p.stdout == b''
    assert p.stderr.startswith(b"The following argument was not expected: --unknown")
