import subprocess

import pytest


def test_merge(xtl_clone, git2cpp_path, tmp_path):
    assert (tmp_path / 'xtl').exists()
    xtl_path = tmp_path / 'xtl'

    checkout_cmd = [git2cpp_path, 'checkout', '-b', 'foregone']
    p_checkout = subprocess.run(checkout_cmd, capture_output=True, cwd=xtl_path, text=True)
    assert p_checkout.returncode == 0

    p = xtl_path / 'mook_file.txt'
    p.write_text('blablabla')

    add_cmd = [git2cpp_path, 'add', 'mook_file.txt']
    p_add = subprocess.run(add_cmd, capture_output=True, cwd=xtl_path, text=True)
    assert p_add.returncode == 0

    commit_cmd = [git2cpp_path, 'commit', '-m', 'test commit']
    p_commit = subprocess.run(commit_cmd, capture_output=True, cwd=xtl_path, text=True)
    assert p_commit.returncode == 0

    checkout_cmd_2 = [git2cpp_path, 'checkout', 'master']
    p_checkout_2 = subprocess.run(checkout_cmd_2, capture_output=True, cwd=xtl_path, text=True)
    assert p_checkout_2.returncode == 0

    merge_cmd = [git2cpp_path, 'merge', 'foregone']
    p_merge = subprocess.run(merge_cmd, capture_output=True, cwd=xtl_path, text=True)
    assert p_merge.returncode == 0

    log_cmd = [git2cpp_path, 'log', '--max-count', '1']
    p_log = subprocess.run(log_cmd, capture_output=True, cwd=xtl_path, text=True)
    assert p_log.returncode == 0
    assert "Jane Doe" in p_log.stdout
    assert (xtl_path / 'mook_file.txt').exists()

# def test_merge_multiple_branches(xtl_clone, git2cpp_path, tmp_path):
#     pass
