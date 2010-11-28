========================================================================
    SNAP GIT REPOSITORY
========================================================================
Quick intro on how to use the repository:

/////////////////////////////////////////////////////////////////////////////
Each branch acts like a separate version of the repository, so all
branches are pushed/pulled to/from the main repository.

If you want to change the branch you are working in you say "git
checkout <branchname>".

"master" is the name of the main (the official) branch. 

To merge branch B with the "master' you say: 
	$git checkout master	// go to master branch
	$git merge B		// merge with branch B
To list a set of branches, say:
	$git branch
To list a set of remote, i.e., branches on the server, say:
	$git branch -r
To subscribe to an existing remote branch we say
	$git checkout -b <local_name> origin/<remote_name>
To make a local branch a remote branch, i.e., add a branch to the 
repository so that we can then push it:
	$git push origin <local_branch>


/////////////////////////////////////////////////////////////////////////////
Example of how to use Git:

So, you start by "checking out" (i.e., cloning the repository)
	$git clone <user>@shark.stanford.edu:/u/snap/git/sandbox.git
We create a local branch
	$git checkout -b jure-branch
Now:
	$git branch
Lists there are two branches in the repository: jure-branch and master
Let's add a file to the local branch "jure-branch" and commit it
	$echo "jure branch" > jure-branch.txt
	$git add jure-branch.txt
	$git commit -m "Jure Branch" -a
Executing
	$git status
reveals we are working on the branch "jure-branch"
Changing branches:
	$git checkout jure-branch
	$ls
	$git checkout master
	$ls
We see that "jure-branch" contains the "jure-branch.txt" while master
does not contain the file. If we do "$git push" now nothing happens
since the branch is local. We need to make the local "jure-branch" into
a remote branch (we only need to do this once per branch):
	$git push origin jure-brach
	$git branch -r
Reveals that the remote repository now contains branch 
"origin/jure-branch"

Now we can easily switch between the "master" and the "jure-branch"
	$git checkout master
	$ls
	$git checkout jure-branch
	$ls
Notice "jure-branch" contains the "jure-branch.txt" file, which is not
part of the "master" branch

Now we clone a new version of the repository:
	$git clone jure@shark.stanford.edu:/u/snap/git/sandbox.git sandbox2
	$git branch -r
reveals the branches of the remote repository
Executing "$git checkout -b jure-branch" would create another local
branch, rather than this, we want to track (i.e., subscribe to) the
remote "jure-branch", so we say:
	$git checkout --track -b jure-branch origin/jure-branch
	$git status
	$ls
Shows that we now have "jure-branch.txt" file (i.e., we checked out the
remote jure-branch).

Now we can simply work on the "jure-branch" and pull/push when needed.
If we want to get the new stuff from "master" to "jure-branch" we say:
	$git checkout jure-branch	// move to jure-branch
	$git merge master		// get the stuff from "master"
Similarly you can update the master:
	$git checkout master
	$git merger jure-branch
	
	