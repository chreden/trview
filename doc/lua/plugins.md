# Plugins

# Required Files
To be recognised by trview a plugin needs to have the following folder structure

```
plugin_folder_name
    manifest.json
    plugin.lua
```

The plugin folder then goes inside one of the user's configured plugin folders.

## manifest.json
The manifest gives some information about the plugin to show to the user. This information will be shown in the Plugins window.

### Example Manifest

```
{
    "name": "Example Plugin",
    "author": "trview doc author",
    "description": "This plugin is used as an example plugin"
}
```

## plugin.lua
This is the Lua file that will be loaded by trview and should contain the plugin script.

# Defined Functions
trview will attempt to call the following functions if they are defined in the plugin script

|Function|Parameters|When is it called?|Purpose|
|--------|----------|------------|-------|
|render_ui|None|During trview UI rendering|Used for drawing any plugin windows required|
|render_toolbar|None|When trview is rendering the toolbar at the bottom of the screen|Used to add a button or other control to the tool`bar - for example if you want a way to open the main plugin window|
|set_enabled|boolean enabled|At plugin load and when user toggles whether plugin is enabled|Allows plugin to set up when enabled or to clean up when disabled|