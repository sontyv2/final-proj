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

    <h2 align="middle">Summary</h2>

    <div class="padded">
        <p>For our final project, we’ll be making an openGL video game. The game will be a first person, 3D maze, in which the player will try to navigate to the maze’s exit. </p>

        <p>Team Members:</p>
        <ul> 
            <li>Sonthaya Visuthikraisee, 26123016.</li>
            <li>Julie Han, 26022682</li>
            <li>Varsha Ramakrishnan, 3031797029</li>
        </ul>

    <h2 align="middle">PROBLEM DESCRIPTION</h2>
        <ul> 
            <li>The problem: creating a video game using OpenGL. We must:
                <ul>
                    <li>Simulate first-person interaction with a maze-like environment</li>
                    <li>Simulate interaction with in-game objects and physical responses (walls, movement, jumping, objects that can be picked up and moved).</li>
                    <li>Simulate camera movement (looking around the world) in addition to player movement. This involves mapping camera rotation to a control system, giving the player the ability to look around for collectibles, etc. One cool power up we were thinking of (as a stretch goal) was allowing the player to switch to bird's eye view.</li>
                    <li>Add in light manipulation. For example, having a light that the player can pick up and move to shine on different objects, reflect off mirrors to hit unreachable areas, etc. One stretch goal for this would be having a light that can shine through objects; for exmaple, if a glass door is in the way, we might give the player the ability to shine their light at the door, making the door disappear.</li>
                    <li>Stretch goal: maze generation to create the world and objective.</li>
                </ul>
            </li>
            <li>Why is it important:
                <ul>
                    <li>Simulate traversing through and interacting with an environment with changing illumination, in real time, is the basis behind all video games. The ability to do this accurately and efficiently is vital to rendering for video games.</li>
                </ul>
            </li>
            <li>Challenges:
                <ul>
                    <li>Simulating physics between 3D objects with OpenGL and C++</li>
                    <li>Simulating lighting of the game world and reflecting it off of objects, as well as tracking where it lands to progress in the game.</li>
                </ul>
            </li>
            <li>Solultions:
                <ul>
                    <li>Add up forces, divide by mass, and multiply by time step to get the change in velocity, and multiply by the time step again to get the change in position.</li>
                    <li>Treat the light ray as both a light source for illumination and an object, and use the laws of reflection and refraction to calculate where the light will travel in the game world.</li>
                </ul>
            </li>
        </ul>

    <h2 align="middle">GOALS AND DELIVERABLES</h2>
        <ul> 
            <li>We expect to create an interactive demo of a maze-like world, rendered in real time, so that players can move around with a first-person viewpoint and interact with objects in the world. There should also be a light source that players can manipulate to illuminate different surfaces. </li>
            <li>We hope to create a demo of a game that involves shining light through and off of objects in the game world and track where the light would hit in order to allow the player to move on to the next level of the maze.</li>
        </ul>

    <h2 align="middle">SCHEDULE</h2>
        <ul> 
            <li>Week 1:
                <ul>
                    <li>Learn OpenGL</li>
                    <li>Create a simple world that we can move the camera in</li>
                    <li>Set up walls and player movement</li>
                    <li>Start adding physics collisions</li>
                </ul>
            </li>
            <li>Week 2:
                <ul>
                    <li>Refine the world physics </li>
                    <li>Add in lighting effects and object textures</li>
                    <li> Create a light source that can be moved with the player and that shines forward like a laser and not diffuse</li>
                    <li>Add in a method for tracking where the light ends up/ reflects off of</li>
                </ul>
            </li>
            <li>Week 3:
                <ul>
                    <li>Refine player-maze interactions</li>
                    <li>Work on additional effects and reflections and refractions</li>
                </ul>
            </li>
            <li>Week 4:
                <ul>
                    <li>Finishing touches and extra mechanics</li>
                    <li>Writeup</li>
                </ul>
            </li>
        </ul>

    <h2 align="middle">RESOURCES</h2>
    <ul>
        <li> <a href = "https://www.3dgep.com/introduction-opengl/"> Basic OpenGL </li>
        <li> <a href ="https://www.researchgate.net/publication/283255927_Developing_Simple_Games_with_OpenGL"> Source/reference code </li>
        <li> <a href = "http://info.ee.surrey.ac.uk/Teaching/Courses/CGI/assignment12_maze/assignment12_maze.html"> A Spec For a 3D Maze Game Assignment: </li>
        <li> <a href = "https://www.codeproject.com/script/articles/articleversion.aspx?aid=192956&av=292999"> Random 3D First Person Maze in OpenGL</li>
        <li> <a href = "http://nehe.gamedev.net/tutorial/loading_and_moving_through_a_3d_world/22003/">Loading and Moving through a 3D world </li>
        <li><a href = "https://stackoverflow.com/questions/8963223/opengl-first-person-camera-rotation-and-translation">First Person Camera Rotation and Translation</li>
        <li><a href = "https://www.haroldserrano.com/blog/anatomy-of-light-in-computer-graphic">Lights in OpenGL </li>
</ul>




</div>
</body>
</html>




