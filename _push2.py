import json
import os
import subprocess
import sys

REPO = "07CC05/astar-path-planning"
ENV = {**os.environ, "MSYS_NO_PATHCONV": "1"}


def gh(args):
    r = subprocess.run(["gh"] + args, capture_output=True, text=True, env=ENV, encoding="utf-8")
    if r.returncode != 0:
        print("FAILED:", " ".join(args))
        print((r.stderr or "")[:600])
        sys.exit(1)
    return r.stdout


def api(path, method="GET", payload=None):
    args = ["api", "-X", method, f"repos/{REPO}{path}"]
    if payload is not None:
        with open("_payload.json", "w", encoding="utf-8") as fp:
            json.dump(payload, fp, ensure_ascii=False)
        args += ["--input", "_payload.json"]
    out = gh(args)
    return json.loads(out) if out.strip() else {}


parent = api("/git/ref/heads/main")["object"]["sha"]
print("parent", parent)

files = [f for f in subprocess.check_output(["git", "ls-files", "-z"]).decode("utf-8").split("\0") if f]
tree = []
for f in files:
    tree.append({"path": f, "mode": "100644", "type": "blob",
                 "content": open(f, "rb").read().decode("utf-8")})

tree_sha = api("/git/trees", "POST", {"tree": tree})["sha"]
print("tree", tree_sha, "files", len(files))

msg = subprocess.check_output(["git", "log", "-1", "--pretty=%B"]).decode("utf-8").strip()
commit_sha = api("/git/commits", "POST", {"message": msg, "tree": tree_sha, "parents": [parent]})["sha"]
print("commit", commit_sha)

api("/git/refs/heads/main", "PATCH", {"sha": commit_sha, "force": True})
print("pushed")
