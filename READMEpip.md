# this is to install new package/module using pipx and using that package/module#

cd /root/.local/share/pipx/venvs
ls
cd #installedpackage/module(torch)
python3 -m venv .
source ./bin/activate

## Make Alias ##
alias torchvm="source ~/.local/share/pipx/venvs/torch/bin/activate"
## Deactivate ##
deactivate
