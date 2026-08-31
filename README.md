## Editor Tab

### Keyboard Shortcuts

* `F1` — Saves the current render pipeline.
* `F5` — Recompiles and recreates the pipeline with any changes made to the shaders.
* `Ctrl + S` — Saves the shader currently being edited.

  * **Important:** The file being saved must have the `.asset` extension.
* `Ctrl + L` — Loads a file into the editor.
* **Drag & Drop** — Dragging a file into the text editor window loads it.

## Material Tab

### Keyboard Shortcuts

* `Ctrl + S` — Saves the current shaders, pipeline, and textures into an `.asset` file.
* **Drag & Drop** — Loads a dragged file and its pipeline, if applicable (`.asset`).

  * **Important:** Only `.obj` and `.asset` files are supported.

# Workflow

## Creating an Asset

1. Open the **Material Editor** by going to the **Material Tab**.
2. Drag and drop an `.obj` file into the Material Editor. This will update the shaders in the **Shader Editor** (Editor Tab).
3. On the left side of the screen, assign a material using the `...` button.

   * By default, the `.obj` loads with a default VS and PS shader containing a texture, but the texture is not sampled.
4. Switch to the **Editor Tab**.
5. Modify the shader to either sample the texture assigned in step 3 or drag and drop a pre-written shader into the editor.

   **Important:** Make sure the correct shader is placed in the correct editor.

### Basic Texture Sampling Example

* **PS:** `<SolutionDir>/Editor/Application/Shaders/HLSLShaders/TexturedDefaultPS.hlsl`
* **VS:** Unchanged.

### Phong Lighting Example

* **VS:** `<SolutionDir>/Editor/Application/Shaders/HLSLShaders/TexturedPhongLightingVS.hlsl`
* **PS:** `<SolutionDir>/Editor/Application/Shaders/HLSLShaders/TexturedPhongLightingPS.hlsl`

6. Press `F5` to recompile, then switch back to the **Material Editor** to observe the changes. Any compilation errors will be output to the command prompt.
7. Once you're happy with the result, press `Ctrl + S` while the **Material Tab** is selected to save the `.asset`.
8. The resulting `.asset` can then be dragged and dropped into the **Object Viewport**.
