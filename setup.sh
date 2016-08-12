export GAZEBO_PLUGIN_PATH=${HOME}/gzmaze/world_plugin/build:\
${HOME}/gzmaze/gui_plugin/build:\
${HOME}/model_plugin/build:\
$GAZEBO_PLUGIN_PATH

export LD_LIBRARY_PATH=${HOME}/gzmaze/world_plugin/build:\
${HOME}/gzmaze/gui_plugin/build:\
${HOME}/gzmaze/model_plugin/build:\
$LD_LIBRARY_PATH

gazebo -u --verbose gzmaze.world
