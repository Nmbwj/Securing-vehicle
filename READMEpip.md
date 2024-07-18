# this is to install new package/module using pipx and using that package/module#

## Configuration before any pipx ##
cd /root/.local/share/pipx/
python3 -m venv ./venvs
##

## installing using PIPX ##
pipx install [packge_name]
##

## Make Alias ##
alias torchvm="source ~/.local/share/pipx/venvs/torch/bin/activate"
##

## Deactivate ##
deactivate
##
