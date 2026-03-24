import subprocess
from uuid import uuid4


def test_push_private_repo(
    git2cpp_path, tmp_path, run_in_tmp_path, private_test_repo, commit_env_config
):
    # Unique branch name to avoid branch name collisions on remote repo.
    branch_name = f"test-{uuid4()}"

    # Start of test follows test_clone_private_repo, then creates a new local branch and pushes
    # that to the remote.
    username = "abc"  # Can be any non-empty string.
    password = private_test_repo["token"]
    input = f"{username}\n{password}"
    repo_path = tmp_path / private_test_repo["repo_name"]
    url = private_test_repo["https_url"]

    clone_cmd = [git2cpp_path, "clone", url]
    p_clone = subprocess.run(clone_cmd, capture_output=True, text=True, input=input)
    assert p_clone.returncode == 0
    assert repo_path.exists()
    # Single request for username and password.
    assert p_clone.stdout.count("Username:") == 1
    assert p_clone.stdout.count("Password:") == 1

    status_cmd = [git2cpp_path, "status"]
    p_status = subprocess.run(status_cmd, cwd=repo_path, capture_output=True, text=True)
    assert p_status.returncode == 0
    assert "On branch main" in p_status.stdout
    assert "Your branch is up to date with 'origin/main'" in p_status.stdout

    checkout_cmd = [git2cpp_path, "checkout", "-b", branch_name]
    p_checkout = subprocess.run(checkout_cmd, cwd=repo_path, capture_output=True, text=True)
    assert p_checkout.returncode == 0

    p_status = subprocess.run(status_cmd, cwd=repo_path, capture_output=True, text=True)
    assert p_status.returncode == 0
    assert f"On branch {branch_name}" in p_status.stdout

    (repo_path / "new_file.txt").write_text("Some text")
    add_cmd = [git2cpp_path, "add", "new_file.txt"]
    p_add = subprocess.run(add_cmd, cwd=repo_path, capture_output=True, text=True)
    assert p_add.returncode == 0

    commit_cmd = [git2cpp_path, "commit", "-m", "This is my commit message"]
    p_commit = subprocess.run(commit_cmd, cwd=repo_path, capture_output=True, text=True)
    assert p_commit.returncode == 0

    log_cmd = [git2cpp_path, "log", "-n1"]
    p_log = subprocess.run(log_cmd, cwd=repo_path, capture_output=True, text=True)
    assert p_log.returncode == 0
    assert p_log.stdout.count("Author:") == 1
    assert p_log.stdout.count("Date:") == 1
    assert p_log.stdout.count("This is my commit message") == 1

    # push with incorrect credentials to check it fails, then with correct to check it works.
    input = f"${username}\ndef\n{username}\n{password}"
    push_cmd = [git2cpp_path, "push", "origin"]
    p_push = subprocess.run(push_cmd, cwd=repo_path, capture_output=True, text=True, input=input)
    assert p_push.returncode == 0
    assert p_push.stdout.count("Username:") == 2
    assert p_push.stdout.count("Password:") == 2
    assert " * [new branch]      test-" in p_push.stdout
    print(p_push.stdout)


def test_push_branch_private_repo(
    git2cpp_path, tmp_path, run_in_tmp_path, private_test_repo, commit_env_config
):
    """Test push with an explicit branch name: git2cpp push <remote> <branch>."""
    branch_name = f"test-{uuid4()}"

    username = "abc"
    password = private_test_repo["token"]
    input = f"{username}\n{password}"
    repo_path = tmp_path / private_test_repo["repo_name"]
    url = private_test_repo["https_url"]

    # Clone the private repo.
    clone_cmd = [git2cpp_path, "clone", url]
    p_clone = subprocess.run(clone_cmd, capture_output=True, text=True, input=input)
    assert p_clone.returncode == 0
    assert repo_path.exists()

    # Create a new branch and commit on it.
    checkout_cmd = [git2cpp_path, "checkout", "-b", branch_name]
    p_checkout = subprocess.run(checkout_cmd, cwd=repo_path, capture_output=True, text=True)
    assert p_checkout.returncode == 0

    (repo_path / "push_branch_file.txt").write_text("push branch test")
    subprocess.run([git2cpp_path, "add", "push_branch_file.txt"], cwd=repo_path, check=True)
    subprocess.run([git2cpp_path, "commit", "-m", "branch commit"], cwd=repo_path, check=True)

    # Switch back to main so HEAD is NOT on the branch we want to push.
    subprocess.run(
        [git2cpp_path, "checkout", "main"], capture_output=True, check=True, cwd=repo_path
    )

    status_cmd = [git2cpp_path, "status"]
    p_status = subprocess.run(status_cmd, cwd=repo_path, capture_output=True, text=True)
    assert p_status.returncode == 0
    assert "On branch main" in p_status.stdout

    # Push specifying the branch explicitly (HEAD is on main, not the test branch).
    input = f"{username}\n{password}"
    push_cmd = [git2cpp_path, "push", "origin", branch_name]
    p_push = subprocess.run(push_cmd, cwd=repo_path, capture_output=True, text=True, input=input)
    assert p_push.returncode == 0
    assert " * [new branch]      test-" in p_push.stdout
    print("\n\n", p_push.stdout)


def test_push_branches_flag_private_repo(
    git2cpp_path, tmp_path, run_in_tmp_path, private_test_repo, commit_env_config
):
    """Test push --branches pushes all local branches."""
    branch_a = f"test-a-{uuid4()}"
    branch_b = f"test-b-{uuid4()}"

    username = "abc"
    password = private_test_repo["token"]
    input = f"{username}\n{password}"
    repo_path = tmp_path / private_test_repo["repo_name"]
    url = private_test_repo["https_url"]

    # Clone the private repo.
    clone_cmd = [git2cpp_path, "clone", url]
    p_clone = subprocess.run(clone_cmd, capture_output=True, text=True, input=input)
    assert p_clone.returncode == 0
    assert repo_path.exists()

    # Create two extra branches with commits.
    for branch_name in [branch_a, branch_b]:
        subprocess.run(
            [git2cpp_path, "checkout", "-b", branch_name],
            capture_output=True,
            check=True,
            cwd=repo_path,
        )
        (repo_path / f"{branch_name}.txt").write_text(f"content for {branch_name}")
        subprocess.run([git2cpp_path, "add", f"{branch_name}.txt"], cwd=repo_path, check=True)
        subprocess.run(
            [git2cpp_path, "commit", "-m", f"commit on {branch_name}"],
            cwd=repo_path,
            check=True,
        )

    # Go back to main.
    subprocess.run(
        [git2cpp_path, "checkout", "main"], capture_output=True, check=True, cwd=repo_path
    )

    # Push all branches at once.
    input = f"{username}\n{password}"
    push_cmd = [git2cpp_path, "push", "origin", "--branches"]
    p_push = subprocess.run(push_cmd, cwd=repo_path, capture_output=True, text=True, input=input)
    assert p_push.returncode == 0
    assert " * [new branch]      test-" in p_push.stdout
    # assert "main" not in p_push.stdout
    print("\n\n", p_push.stdout)
