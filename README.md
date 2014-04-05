school-rsff
===========

simulation of a wireless array of sensors

This project lives on http://github.com/alzwded/school-rsff

Video of the simulator in action:
```
http://youtu.be/Yq9bAg_VTa4
```

Interacting with the simulator
==============================

Running
-------

To run from a command line, do:
```
rsff inputmodel
```
where `inputmodel` is a well formatted text file described below.

The model format
----------------

The model text file describes a wireframe building and positions 3 types of transmitters/receivers: sensors (emitter-only), routers, and cetral nodes (receiver-only).

The format in BNF is:
```
input-file:
          | input-list
          ;

input-list: input-list building-section
          | input-list sensor-section
          | input-list remark
          | building-section
          | sensor-section
          | remark
          ;

COORD: '(' FLOAT ',' FLOAT ',' FLOAT ')' /* (x, y, z) */

building-section: "BUILDING" beam-decl ;

beam-decl: COORD COORD ;

sensor-section: "SENSOR" sensor-decl ;

sensor-decl: sensor-type COORD FLOAT ; /* type, location, range */

sensor-type: "CENTRAL" /* nodul coordonator */
           | "ROUTER"  /* router wireless   */
           | "SENSOR"  /* senzor efectiv    */
           ;

remark: "REM[^$]*$" ;
```

Look at `testinput` for an example.

Camera manipulation
-------------------

| Action                        | Outcome                           |
|-------------------------------|-----------------------------------|
| Left Mouse Button + Drag      | Rotate camera                     |
| Right Mouse Button + Drag     | Move cameraman on the XZ plane    |
| Middle Mouse Button + Drag    | Move cameraman along the Y axis   |

Running an animation
--------------------

You need to select a starting sensor with the two buttons in the lower-left corner. The currently selected sensor is highlighted in lime green.

Then, you can use the start/stop simulation button (lower right corner) to run the animation.

You can turn the range-sphere's visibility on or off by clicked on the show range button (bottom right)
