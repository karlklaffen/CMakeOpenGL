*Italicized fields are optional*

## TEXTURES  

texture name  
- source
    > *file name of texture image*
- *filters*
    - *min* (string) = "LINEAR_MIPMAP_LINEAR"
        > *minification filter for texture:*  
        > ***"LINEAR_MIPMAP_LINEAR"***  
        > ***"LINEAR_MIPMAP_NEAREST"***  
        > ***"NEAREST_MIPMAP_NEAREST"***  
        > ***"NEAREST_MIPMAP_LINEAR"***  
        > ***"LINEAR"***  
        > ***"NEAREST"***  
    - *mag* (string) = "LINEAR"
        > *magnification filter for texture:*  
        > ***"LINEAR"**,*  
        > ***"NEAREST"***

## MODELS

model name
- source (string)
    > *name of folder containing model data/textures*

## CUBEMAPS

cubemap name
- textures (string)
    > *name of folder containing cubemap textures*

## FONTS

font name
- source (string)
    > *name of font file*
- height (unsigned int)
    > *height of font*  
    > *acts as control for font size*

## SHADERS

shader name
- vertex (string)
    > *file name of vertex shader*
- fragment (string)
    > *file name of fragment shader*

## INTERFACES  

interface name
- *cursor mode* (string) = "NORMAL"
    > *Mode cursor should be in while interface is active:*  
    > ***NORMAL** = cursor shows*  
	> ***HIDDEN** = cursor hidden when over window*  
	> ***DISABLED** = cursor hidden, moves freely*  
- *textures*  
    - *widget*
        > *if not specified interface is assumed to have no widgets*  
        - name (string)
            > *name of texture that widgets will pull their texture from*  
    - *background*
        > *if not specified interface is assumed to have no background*
        - name (string)
            > *name of background texture that will cover screen and render behind other UI elements*  
        - *warp* (bool)
            > *if **true**, aspect ratio of texture will change based on screen aspect ratio so that entire texture is visible*  
            > *if **false**, edges of texture (top/bottom or left/right) will be cut out, allowing for texture aspect ratio to remain constant*
            > *if unspecified, defaults to **false***
- *widgets*
    - *for each list element:*
        - name (string)
            > *reference name of widget*
        - parameters
            - function (string)
                > *name of registered widget function to execute on interaction*
            - *anchor*
                > *sets position relative to center or edge of screen*
                - *x*
                    > ***"LEFT"***  
                    > ***"CENTER"***  
                    > ***"RIGHT"***
                    > *when value is **LEFT**, there is effectively no anchor, meaning that only **x** in **transformation** controls x coordinate*
                - *y*
                    > ***"BOTTOM"***  
                    > ***"CENTER"***  
                    > ***"TOP"***  
                    > *when value is **BOTTOM**, there is effectively no anchor, meaning that only **y** in **transformation** controls y coordinate*
            - transformation
                > *on-screen transformation of widget*  
                > *x and y act as offsets to initial position controlled by **anchor***
                - x (int)
                - y (int)
                - width (unsigned int)
                - height (unsigned int)
            - *texture sources*
                - *for each list element:*
                    - name (string)
                        > *reference name of this texture source*  
                        > *if name of this source is equal to the name of a widget action (**"HOVER"**, **"UNHOVER"**, **"PRESS"**, or **"UNPRESS"**), the source will be switched to on that corresponding action*
                    - x (unsigned int)
                    - y (unsigned int)
                    - width (unsigned int)
                    - height (unsigned int)
                        > *reference area to source texture from large widget texture*
            - *initial source name* (string)
                > *name of initial texture source to use for widget*  
                > *if blank or unspecified, widget is initially invisible yet interactable*
- *texts*
    - *for each element*
        - name (string)
            > *reference name of text object*
        - parameters
            - font (string)
                > *name of font to use*
            - string (string)
                > *initial string for text to display*
            - color (vec3)
                > *color of displayed text*
            - *anchor*
                > *sets position relative to center or edge of screen*
                - *x*
                    > ***"LEFT"***  
                    > ***"CENTER"***  
                    > ***"RIGHT"***  
                    > *when value is **LEFT**, there is effectively no anchor, meaning that only **pos** controls x coordinate*
                - *y* (string) = "BOTTOM"
                    > ***"BOTTOM"***  
                    > ***"CENTER"***  
                    > ***"TOP"***  
                    > *when value is **BOTTOM**, there is effectively no anchor, meaning that only **pos** controls y coordinate*
            - pos (vec2)
                > *position to place text on screen*  
                > *if anchor is not NONE, acts as an offset to anchor position*
            - *scale* (unsigned int) = 1
                > *scale to render text at (multiplied by baked-in font size)*
            - *line spacing* (unsigned int) = 10
                > *vertical distance between baseline of a line of text and top of a line of text directly below*  
                > *defaults to 10 if unspecified*
- *fields*
    - *for each element*
        - name (string)
            > *reference name for field object*
        - parameters
            - reference (string)
                > *name of text object to reference for this field*
            - cursor
                - *movable* (bool) = false
                    > *whether or not the cursor should be positioned anywhere except for the end of the text string*  
                - *blink hold time* (float) = 0.5
                    > *time in seconds the cursor should be visible/invisible for a blink*  
                    > *a blink time of 0 indicates that the cursor should stay visible and not blink*
            - *max chars* (unsigned int) = 99999
                > *the maximum number of characters the field's text can hold*  
            - *text cursor mode* (string) = "BAR"
                > *type of cursor the field should have:*  
                > ***"BAR"** = standard vertical bar,*  
                > ***"OVERLAY"** = overlay over character (command line-style)*  

## SCENES

scene name
- lighting (string)
    > *lighting type to use for models in scene:*  
    > ***BLINN_PHONG**,*  
    > ***PBR***
- *fog* (bool) = false
    > *whether fog should be present in scene*
- *background* (vec3) = [1.0, 1.0, 1.0]
    > *background color for scene*
- *lights*
    - *for each element*
        - pos
            > *position for light in scene*
        - color
            > *color of light in scene*
- *cameras*
    - *for each element*
        - pos (vec3)
            > *position of camera in scene*
        - *yaw* (float) = 0.0
            > *rotation left/right, when facing forward*
        - *pitch* (float) = 0.0
            > *rotation up/down, when facing forward*
        - *zoom* (float) = 45.0
            > *vertical FOV of camera*
- *models*
    - *for each element*
        - name
            > *reference name of model*
        - instances
            - *for each element*
                - *translation* (vec3) = none
                    > *position of model in scene*
                - *rotation* = none
                    - vector (vec3)
                        > *vector for which to rotate around*
                    - angle (float)
                        > *angle of rotation, in degrees*
                - *scale* (vec3) = none
                    > *scaling in XYZ of model*
- *cubemaps* (vector)
    - *names of cubemaps in scene*