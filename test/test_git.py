import subprocess

def test_version():
    cmd = ['build/git2cpp', '-v']
    p = subprocess.run(cmd, capture_output=True)
    assert p.returncode == 0
    assert len(p.stderr) == 0
    assert p.stdout.startswith(b'git2cpp ')

def test_unknown_option():
    cmd = ['build/git2cpp', '--unknown']
    p = subprocess.run(cmd, capture_output=True)
    #assert p.returncode == 1
    assert len(p.stdout) == 0
    assert p.stderr.startswith(b"The following argument was not expected: --unknown")
