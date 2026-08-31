# Editor
Work in progress editor

# Usage
## Key binginds
### Editor Tab
F1 - Saves the current render pipeline.

F5 - Re compiles and recreates the pipeline with an changes made to the shaders.

CTRL + S - Saves the shader you are currently looking at. 
  - IMPORTANT: the file that is saved must be the extentsion .asset.

CTRL + L - Loads a file into the editor your currently looking at.

Drag & Drop - dragging a file into the text editor window loads it in.

### Material Tab
CTRL + S - Saves the current shaders, pipeline ,textures into an asset.

Drag & Drop - Loads a Dragged file and its pipeline if applicable (.asset). 
  - IMPORTANT: Only supports .obj or .asset files.
 
# Workflow
## Creating an asset.
- Open up the Material editor by going to the material tab.

- Drag and drop in an .obj file. this will update the Shaders in the Shader editor (Editor Tab)

- on the left assign a material using the ... button on the left of the screen.
  - by default the .obj loads in with a default vs and ps shader which contains a texture but the texture isnt sampled.

- swap over to the Editor tab.

- Modify the shader to either Sample the texture you assigned in step 3 or drag and drop a pre written shader into the editor.

  IMPORTANT: Make sure that the correct shader is placed in the correct editor.
  ### Basic texture sampling example
  - PS: <SolutionDir>/Editor/Applicatiob/Shaders/HLSLShaders/TexturedDefaultPS.hlsl
  - VS: Unchanged.
  ### Phong lighting example.
  - VS: <SolutionDir>/Editor/Applicatiob/Shaders/HLSLShaders/TexturedPhongLightingVS.hlsl
  - PS: <SolutionDir>/Editor/Application/Shaders/HLSLShaders/TexturedPhongLightingPS.hlsl

- press F5 to recompile and move back into the material editor observe the changes. any errors will be output the the command prompt window.
- once happy press ctrl + s while the Material tab is selected to save the .asset which then can be drag and droped into the Object viewport.
  
