<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
<head>
<style>  
    div.padded {  
      padding-top: 0px;  
      padding-right: 100px;  
      padding-bottom: 0.25in;  
      padding-left: 100px;  
    }  
    body {
        font-weight: 300;
        font-family: 'Open Sans', sans-serif;
        color: #121212;
    }
  h1, h2, h3, h4 {
    font-family: 'Source Sans Pro', sans-serif;
  }
</style> 
<title>Sonthaya Visuthikraisee, Julie Han, and Varsha Ramakrishnan |  CS 184</title>
<meta http-equiv="content-type" content="text/html; charset=utf-8" />
<link rel="stylesheet" type="text/css" href="style.css" media="screen" />
<link href="https://fonts.googleapis.com/css?family=Open+Sans|Source+Sans+Pro" rel="stylesheet">

</head>
<body>
<br />
<h1 align="middle">CS 184: Computer Graphics and Imaging, Spring 2018</h1>
<h1 align="middle">A-maze-ing game with corny puns</h1>
<p align="middle">Sonthaya Visuthikraisee, 26123016</p>
<p align="middle">Julie Han, 26022682</p>
<p align="middle">Varsha Ramakrishnan, 3031797029</p>

<div class="padded">

<h2 align="middle">Abstract</h2>
<!-- A paragraph summary of the entire project. -->

<p>For our final project, we created an OpenGL video game. The game is a first person 3D maze in which the player tries to navigate to the maze’s exit. The maze is procedurally generated and consists of three different wall types: normal walls, mirror walls, and glass walls. There are also 5 powerups that are randomly placed throughout the maze that give the player the following abilities: viewing the maze from a bird's-eye view, highlighting glass walls, highlighting mirror walls, turning mirror walls opaque, and turning glass walls opaque. Finally, the player is provided with a laser pointer that follows the direction the player is facing. The laser interacts with the mirror and glass walls, bouncing off and passing through them respectively.</p>


<h2 align="middle">Technical approach</h2>
<!-- A 1-2 page summary of your technical approach, techniques used, algorithms implemented, etc. (use references to papers or other resources for further detail). Highlight how your approach varied from the references used (did you implement a subset, or did you change or enhance anything), the unique decisions you made and why.
A description of problems encountered and how you tackled them.
A description of lessons learned. -->

<h3 align="left">Maze generation</h3>

<p> The maze is procedurally generated using a <a href="https://rosettacode.org/wiki/Maze_generation#Python">Python script</a> that runs the following DFS Algorithm: </p>
<ul>
    <li>Starting with a grid of individually walled-off cells, the algorithm randomly traverses through the cells, removing walls as it goes. It stops the traversal when a cell has no unvisited neighbors (i.e. a dead end has been reached). </li>
    <li> Upon reaching a dead end, the algorithm backtracks until there is a cell with an unvisited neighbor and continues traversing from there. </li>
    <li> The process is repeated until all cells have been visited. </li>
</ul>

<p> The source code has been modified in the following ways: </p>
<ul>
    <li>Corridors are represented by a 0, opaque walls by a 1, mirror walls by a 2, and glass walls by a 3.</li>
    <ul>
        <li>The boundaries of the maze are always made of opaque walls; otherwise, a wall has a 1/2 chance of being opaque, 1/4 of being a mirror, and 1/4 chance of being glass.</li>
    </ul>
    <li>An entrance and exit have been added to the maze.</li>
    <li>The algorithm randomly places 4 powerups in the maze, which are represented by a 4.</li>
    <ul>
        <li>The first powerup is always located at the entrance of the maze, so it is not randomly placed by the maze generation algorithm.</li>
    </ul>
</ul>

<div align="center">
    <table style="width=100%">
        <tr>
            <td>
                <img src="images/maze.PNG" width="480px" height="360"/>
                <figcaption align="middle">Original source code output.</figcaption>
            </td>
            <td>
                <img src="images/finalmaze.PNG" width="480px" height="360"/>
                <figcaption align="middle">Modified code output.</figcaption>
            </td>                
        </tr>         
    </table>
</div>

<p>The next step was integrating this Python script with the rest of our C++ code. The main C++ file calls the Python script, which writes its output into a text file. In the C++ file, the text file is read and converted into 3D coordinates of where the walls/powerups will be placed in the scene. Additionally, there is an array that stores the type of each wall so the appropriate shader effect (opaque/mirror/glass) is applied when the walls are rendered.</p>

<h3 align="left">First-person camera movement</h3>

<p>The matrix representing the <a href="https://learnopengl.com/Getting-started/Camera">view/camera space</a> is derived from the following vectors: the camera position, the camera direction, the right axis, and the up axis. Given the position, direction, and up vectors, GLM's lookAt function creates this matrix for us. By modifying the camera position by a constant distance scaled by some speed every time a direction key &lt;WASD&gt; is pressed, we create the "walking around" effect of the camera. The "looking around" effect of the mouse is created by making use of the pitch and yaw Euler angles. This accomplished by finding the offset of the mouse's position from the last frame from which the pitch and yaw is derived, constraining the pitch values, and calculating the direction vector.</p>

<div align="center">
    <table style="width=100%">
        <tr>
            <td>
                <video src="images/camerademo.mov" controls="controls" width="480" height="360" name="First-person camera movement."> </video>
                <figcaption align="middle">First-person camera movement (from milestone report).</figcaption>
            </td>             
        </tr>    
    </table>
</div>

<h3 align="left">Wall/item collisions and powerups</h3>

<p>We modified the camera code to accept the wall/powerup positions and sizes as inputs. After the camera movement step has been calculated, we check if the new camera position intersects any of the walls or powerups. If it does, then the camera position reset to its original position plus a small bounce factor, which produces the effect of the player being pushed back by the object.</p>

<p>The powerup pickup algorithm is implemented in a similar manner. When the player presses "E," we check if the camera position is sufficiently close to a powerup. If it is, then the nearest powerup to the player is removed from the scene.</p>

<p>Key presses are tracked with booleans. Depending on the number of powerups the player has picked up, certain keys toggle the following effects:</p>
<ul>
    <li>First powerup: "Y" for bird's-eye view.</li>
    <ul>
        <li>The camera position is set to a fixed point above the maze. The player can look around with the mouse, but normal movement with "WASD" is not allowed.</li>
    </ul>
    <li>Second powerup: "I" to highlight glass walls.</li>
    <ul>
        <li>When rendering, glass walls are rendered with a modified shader that darkens their appearance.</li>
    </ul>
    <li>Third powerup: "U" to highlight mirror walls.</li>
    <ul>
        <li>When rendering, mirror walls are rendered with a modified shader that brightens their appearance.</li>
    </ul>
    <li>Fourth powerup: "J" to turn mirror walls opaque.</li>
    <ul>
        <li>When rendering, mirror walls are rendered with the same shader as opaque walls.</li>
    </ul>
    <li>Fifth powerup: "K" to turn glass walls opaque.</li>
    <ul>
        <li>When rendering, mirror walls are rendered with the same shader as opaque walls.</li>
    </ul>
</ul>

<h3 align="left">Shaders/Textures for the maze environment</h3>

<p>All vertex shaders used for the walls and powerups take in view, model, and projection uniform matrices which are used to map from object space to screen space.</p>

<p>The shaders used for the powerup items are similar to the starter code for the extra credit portion from <a href="https://cs184.eecs.berkeley.edu/article/28">Project 3-2, Part 5</a>. The color is calculated in the vertex shader as a sin/cos function of time. The same color is passed into the fragment shader, which simply outputs it. The rotation of the powerup is calculated in a similar manner based on time. A rotation matrix calculated in the main C++ file. First, a spin is calculated based on the time; this is used as an Euler angle, converted into a quaternion, and finally converted to a 4x4 matrix. This matrix is passed into the vertex shader as a uniform, which then applies the rotation matrix to the position of the vertex.</p>

<p>The opaque walls use fairly basic <a href="https://cs184.eecs.berkeley.edu/article/28">texture</a> <a href="https://learnopengl.com/Getting-started/Textures">mapping</a>. The texture coordinates are mapped onto a corresponding vertex on the 3D object. The color of the walls are slightly altered from the original texture by adding a constant value to the output color in the fragment shader.</p>

<p>The background, mirror walls, and glass walls all make use of <a href="https://learnopengl.com/Advanced-OpenGL/Cubemaps">cubemapping</a>. A <a href="https://www.youtube.com/watch?v=xutvBtrG23A">cubemap</a> is composed of 6 textures that form a cube that completely captures an environment in all directions.</p>

<p>The background is a skybox: a cube that encompasses the entire scene. The cubemap is mapped onto this giant cube and gives the illusion that the player is in a large environment. The shaders are fairly straightfoward, the one point of note being that the position vector passed into the vertex shader is then output as the texture coordinate passed to the fragment shader. For optimization purposes, the skybox is rendered last, which is possible by setting it at maximum depth.</p>

<p>The mirror and glass walls make use of environment mapping of the cubemap. In the reflection fragment shader for the mirror effect, the vector from the view/camera position to the object is calculated and reflected across the surface normal; the skybox is then sampled in the direction of the reflected vector for the final output fragment color. Similarly, the refraction fragment shader refracts the vector according to the ratio of the index of refraction (in the case of glass, 1.00 / 1.52) and samples the skybox for color accordingly.</p>

<p>To produce the highlighting effects of the powerups, the reflection and refraction fragment shaders are modified to alter the color output of the fragment. This is achieved by adding/subtracting a constant value to the output color.</p>

<div align="center">
    <table cellpadding="10" style="width=100%">
        <tr>
            <td>
            </td>
            <td>
                <video src="images/PartyBox2.mov" controls="controls" width="360" height="360" name="Party box."> </video>
                <figcaption align="middle">A powerup.</figcaption>
            </td>
        </tr>
        <tr>
            <td>
                <video src="images/texturedbox2.mov" controls="controls" width="360" height="360" name="Normal box."> </video>
                <figcaption align="middle">An opaque textured box.</figcaption>
            </td>   
            <td>
                <video src="images/mirrorbox.mp4" controls="controls" width="360" height="360" name="Mirror box."> </video>
                <figcaption align="middle">A reflective mirror box.</figcaption>
            </td>
            <td>
                <video src="images/glassbox.mp4" controls="controls" width="360" height="360" name="Glass box."> </video>
                <figcaption align="middle">A refractive glass box.</figcaption>
            </td>           
        </tr>
    </table>
</div>

<h3 align="left">Laser</h3>

<h3 align="left">Challenges</h3>

<h4 align="left">Setting up and integrating code</h4>

<p>A major obstacle we encountered during the project was setting up OpenGL and all the necessary packages/dependencies in Visual Studio. This was especially frustrating when team members were working on separate parts of the project, and we would encounter countless errors due to missing dependencies/files when attempting to integrate code. One of our team members had additional difficulties in getting Visual Studio set up properly on her Mac. Once we finally sorted out all the packages and files we would need to proceed with the rest of the project, the process became much smoother.</p>

<h4 align="left">Dynamic environment mapping</h4>

<p>The mirror and glass walls in the maze do not display the other objects in the maze in their reflections/refractions because the shaders use the cubemap of the environment, which does not contain those objects in its textures. In trying to include the other objects in the reflections, we looked into <a href="https://www.youtube.com/watch?v=lW_iqrtJORc">dynamic environment mapping</a>, which dynamically generates cubemaps of the scene as its being rendered. However, this process is extremeley computationally intensive, since a cubemap must be rendered for each reflective/refractive object for it to look correct; in other words, the entire scene must be rendered 6 times per object. One potential workaround is pre-rendering cubemaps; however, our maze environment is randomly generated, so pre-rendering was not an option. Ultimately, we decided to retain the basic environment mapping of the skybox.</p>

<h4 align="left">Laser</h4>

<p>Our first idea for implementing a laser was to draw a <a href="https://www.gamedev.net/forums/topic/162583-lasers-in-opengl/">simple line</a> whose appearance we could later modify with textures; however, we found that modern versions of OpenGL (like the one we are using) no longer supports line drawing capabilities. We then found <a href="https://www.youtube.com/watch?v=bmblOymqfD8">other</a> <a href="https://gamecollage.com/how-to/lasers-and-mirrors/">resources</a> for implementing lasers in OpenGL, but the source code used a different package (GLEW) than the one we had been using for the rest of the project (GLAD), which caused a lot of integration issues.</p>

<h3 align="left">Lessons learned</h3>

<p>The main lesson we learned is that graphics is difficult, even with established packages and utilities, but highly rewarding. While the process of setting up the project, linking all the packages together, getting past the learning curve for OpenGL, and integrating different parts of code was frustrating, being able to see and play with our final results made the effort worth it.</p>

<p>On the more technical side, we learned the power of OpenGL and its ability to render in real-time, as well as the flexibility of shaders in changing the appearance of objects and the viewing perspective of the rendered scene. We also reinforced physics concepts learned in class, such as reflection and refraction for mirror/glass objects, the behavior of light with such objects, and detecting collisions and intersections.</p>

<h2 align="middle">Results</h2>
<!-- Your final images, animations, video of your system (whichever is relevant). You can include results that you think show off what you built but that you did not have time to go over on presentation day. -->

<p align=middle>
    <video src="images/" controls="controls" width="480" height="360" name="Going through the maze."> </video>
    <figcaption align="middle">Final video.</figcaption>
</p> 

<div align="center">
    <table cellpadding="10" style="width=100%">
        <tr>
            <td>
                <video src="images/camsprint.mp4" controls="controls" width="360" height="360" name="Camera movement."> </video>
                <figcaption align="middle">Camera movement (WASD+mouse), sprint (left shift).</figcaption>
            </td>
            <td>
                <video src="images/bumppickup.mp4" controls="controls" width="360" height="360" name="Collision and pickup."> </video>
                <figcaption align="middle">Collisions and item pickup (E).</figcaption>
            </td>
            <td>
                <video src="images/bird.mp4" controls="controls" width="360" height="360" name="Bird's-eye view."> </video>
                <figcaption align="middle">Bird's-eye view (Y).</figcaption>
            </td>
        </tr>
            <td>
                <video src="images/" controls="controls" width="360" height="360" name="Laser toggle."> </video>
                <figcaption align="middle">Toggling the laser.</figcaption>
            </td>
            <td>
                <video src="images/" controls="controls" width="360" height="360" name="Laser + mirror."> </video>
                <figcaption align="middle">Laser-mirror interaction.</figcaption>
            </td>   
            <td>
                <video src="images/" controls="controls" width="360" height="360" name="Laser + glass."> </video>
                <figcaption align="middle">Laser-glass interaction.</figcaption>
            </td>
        <tr>
        </tr>
        <tr>
            <td>
                <video src="images/mirrorlight.mp4" controls="controls" width="360" height="360" name="Mirror powers."> </video>
                <figcaption align="middle">Mirror highlight (U).</figcaption>
            </td>
            <td>
                <video src="images/glasslight.mp4" controls="controls" width="360" height="360" name="Glass powers."> </video>
                <figcaption align="middle">Glass highlight (I).</figcaption>
            </td>
            <td>
                <video src="images/mirrorsolid.mp4" controls="controls" width="360" height="360" name="Mirror powers."> </video>
                <figcaption align="middle">Mirror->opaque (J).</figcaption>
            </td>
        </tr>
        <tr>
            <td>
                <video src="images/glasssolid.mp4" controls="controls" width="360" height="360" name="Mirror powers."> </video>
                <figcaption align="middle">Glass->opaque (K).</figcaption>
            </td>
            <td>
                <video src="images/powers.mp4" controls="controls" width="360" height="360" name="Mirror powers."> </video>
                <figcaption align="middle">Demo all powerups.</figcaption>
            </td>
            <td>
                <video src="images/" controls="controls" width="360" height="360" name="Going through the maze."> </video>
                <figcaption align="middle">Traversing through the maze.</figcaption>
            </td>  
        </tr>
    </table>
</div>

<h2 align="middle">References</h2>

<ul>
    <li> <a href="https://www.youtube.com/watch?v=k9LDF016_1A">Setting up Visual Studio, OpenGL, GLFW, GLAD</a> </li>
    <li> <a href="https://rosettacode.org/wiki/Maze_generation#Python">Maze generation code</a> </li> 
    <li> <a href="https://cs184.eecs.berkeley.edu/article/28">Project 3-2, Part 5</a> </li>
    <li> <a href="https://learnopengl.com/Getting-started/Camera">OpenGL tutorial</a> </li>
    <ul>
        <li> <a href="https://learnopengl.com/Getting-started/Camera">First person camera tutorial</a> </li>
        <li> <a href="https://learnopengl.com/Advanced-OpenGL/Cubemaps">Cubemap tutorial</a> </li>
    </ul>
    <li> <a href="https://www.youtube.com/watch?v=xutvBtrG23A">Cubemap reflections tutorial</a> </li>
    <li> <a href="https://www.youtube.com/watch?v=lW_iqrtJORc">Rendering to cube map texture tutorial</a> </li>
    <li> <a href="https://www.youtube.com/watch?v=bmblOymqfD8">Lasers: Video demo + high level overview</a> </li>
    <li> <a href="https://gamecollage.com/how-to/lasers-and-mirrors/">Lasers: High level overview</a> </li> 
    <li> <a href="https://www.gamedev.net/forums/topic/162583-lasers-in-opengl/">Simple laser implementation</a> </li>
    <li> <a href="https://screencast-o-matic.com/screen_recorder">Screen recording app</a> </li>
</ul>

<h2 align="middle">Contributions</h2>
<!-- A clear description of the work contributed by each team member. -->

<div align="center">
    <table border="1" cellpadding="5" style="width=100%">
        <tr>
            <td width=480px align="center">
                <p> <b> Sonty </b> </p>
            </td>
            <td width=480px align="center">
                <p> <b> Julie </b> </p>
            </td>     
            <td width=480px align="center">
                <p> <b> Varsha </b> </p>
            </td>      
        </tr>
        <tr>
            <td>
                <ul>
                    <li> Modifications to maze generation algorithm </li>
                    <ul>
                        <li> Converted output to numbers representing object type </li>
                        <li> 1/4 chance of wall being mirror or glass </li>
                        <li> Border of maze must be opaque </li>
                    </ul>
                    <li> Implemented function to call Python script from main C++ file </li>
                    <li> Parsing the .txt file output by the maze generation script </li>
                    <li> Implemented function that checks wall type based on output of maze generation script </li>
                    <li> Converting generated maze to 3D coordinates </li>
                    <li> Project Proposal writeup </li>
                    <li> Project Milestone video </li>
                    <li> Project Final Presentation slides </li>
                    <li> Project Final video </li>
                </ul>
            </td>
            <td>
                <ul>
                    <li> Modifications to maze generation algorithm </li>
                    <ul>
                        <li> Added entrance and exit </li>
                        <li> Converted output to numbers representing object type </li>
                    </ul>
                    <li> Converting generated maze to 3D coordinates </li>
                    <li> Implemented sprint </li>
                    <li> Cubemapping for skybox environment </li>
                    <li> Reflective/refractive shaders for mirror/glass effects </li>
                    <li> Modified shaders for mirror/glass powerup effects </li>
                    <li> Powerup collision implementation </li>
                    <li> Time-based shaders for color/rotating effect of powerups </li>
                    <li> Project Milestone writeup </li>
                    <li> Project Milestone video </li>
                    <li> Project Final Presentation slides </li>
                    <li> Project Final writeup </li>
                    <li> Project Final video </li>
                </ul>
            </td>     
            <td>
                <ul>
                    <li> First person camera implementation </li>
                    <li> Basic texture mapping shaders for opaque walls </li>
                    <li> Wall collision implementation </li>
                    <li> Powerup keybindings for toggling </li>
                    <li> Bird's-eye view powerup implementation </li>
                    <li> Implementing switching shaders for other powerups </li>
                    <li> Time-based shaders for color/rotating effect of powerups </li>
                    <li> Implemented laser </li>
                    <ul>
                        <li> Keybinding for toggling the laser on/off </li>
                        <li> Laser follows camera movement </li>
                        <li> Laser reflects off mirrors </li>
                        <li> Laser refracts through glass </li>
                    </ul>
                    <li> Project Proposal writeup </li>
                    <li> Project Milestone video </li>
                    <li> Project Final video </li>
                </ul>
            </td>
        </tr>            
    </table>
</div>


<h2 align="middle">Miscellaneous Links</h2>
<ul>
    <li> Project Webpages </li>
    <ul>
        <li> <a href="https://sontyv2.github.io/final-proj/">Index</a> </li>
        <li> <a href="https://sontyv2.github.io/final-proj/proposal">Proposal</a> </li>
        <li> <a href="https://sontyv2.github.io/final-proj/milestone">Milestone</a> </li>
    </ul>
    <li> Milestone Materials </li>
    <ul>
        <li> <a href="https://docs.google.com/presentation/d/18A6RWoynX8x10vN3lHaGwuzIeagN_B3jkSTHqb1H-lk/edit?usp=sharing">Milestone Slides</a> </li>
        <li> <a href="https://vimeo.com/266222074">Milestone Video</a> </li>
    </ul>
    <li> Final Materials </li>
    <ul>
        <li> <a href="https://docs.google.com/presentation/d/18wJWivD4BIrIFsu4uwsvFUZz0g-dOqWHWcZngtSlOIw/edit?usp=sharing">Final Presentation Slides </a> </li>
        <li> <a href="">Final Video </a> </li>
    </ul>
</ul>



</div>
</body>
</html>




