Set-Location 'C:\GitHub\hido'

Write-Host 'Fetching remotes...'
git fetch --all

$hash = (git rev-parse --short HEAD).Trim()
$backup = "backup/all-branches-$hash"
Write-Host "Creating backup branch: $backup"
git branch -f $backup HEAD
Write-Host "Pushing backup branch to origin: $backup"
git push -u origin $backup

Write-Host 'Resetting master to origin/master'
git fetch origin
# Force local master to match origin/master
git branch -f master origin/master
git checkout master
git reset --hard origin/master

Write-Host 'Deleting local branches except master'
$locals = git for-each-ref --format='%(refname:short)' refs/heads | Where-Object { $_ -ne 'master' }
foreach ($b in $locals) {
    Write-Host "Deleting local branch: $b"
    git branch -D $b
}

Write-Host 'Deleting remote branches except origin/master, origin/HEAD and origin/backup/*'
$remotes = git for-each-ref --format='%(refname:short)' refs/remotes/origin | Where-Object { $_ -ne 'origin/master' -and $_ -ne 'origin/HEAD' -and -not ($_.StartsWith('origin/backup/')) }
foreach ($r in $remotes) {
    $bn = $r -replace '^origin/',''
    Write-Host "Deleting remote branch: $bn"
    git push origin --delete $bn
}

Write-Host 'Local branches now:'
git branch -a
Write-Host 'Remote heads:'
git ls-remote --heads origin
