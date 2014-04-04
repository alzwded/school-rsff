Cerințe
=======

```no-highlight
7. Modelare 3D a unei rețele de senzori wireless, distribuită într-o clădire de
birouri:
 se va realiza/utiliza un plan 3D a unei clădiri de birouri;
 se va amplasa (desena) o rețea de senzori care să realizeze
     monitorizarea clădirii – în vederea măsurării vibrațiilor din structura de
           rezistență a clădirii;
 se va realize o simulare, în măsura posibilităților, a căii parcurse de un
        mesaj de la un nod terminal la nodul coordonator;
```

Adică:
* investigate vibro-acoustic sensors
* realizarea planului 3D
  - posibilitate de input
  - rendare
* amplasarea senzorilor
  - input
    + nod coordonator
    + sensor
  - rendare
* animație
  - input start node
  - pass signal
    + animation frame/step
    + render frame
    + animation speed

Design
======

The building is wireframe, made up of beams. Thus, the user needs to input a pair of 3D coordinates representing those beams.

There are at least 2 types of entities that need to be places. Sensors and coordinating nodes (e.g. routers or end-point). One is defined by its location and its type. The user shall be able to input the coordinate and type of these nodes. They shall be rendered as differently coloured cubes.

[Optional] you can specificy other parameters for the nodes: range, delay, battery, etc.

For the animation, the user shall be able to select a starting point and watch the animation unfold. The animation is defined by the animation speed with the posibility to play/pause. [Optional] rewind.

TODO investigate how wireless sensors work.

File formats
============

In BNF:

```
input-file:
          | input-list
          ;

input-list: input-list building-section
          | input-list sensor-section
          | building-section
          | sensor-section
          ;

COORD: '(' FLOAT ',' FLOAT ',' FLOAT ')' /* (x, y, z) */
```

Building
--------

```
building-section: "BUILDING" coord-list ;

coord-list: coord-list beam-decl
          | beam-decl
          ;

beam-decl: COORD COORD ;
```

Sensors
-------

```
sensor-section: "SENSORS" sensor-list ;

sensor-list: sensor-list sensor-decl
           | sensor-decl
           ;

sensor-decl: sensor-type COORD FLOAT ; /* type, location, range */

sensor-type: "CENTRAL" /* nodul coordonator */
           | "ROUTER"  /* router wireless   */
           | "SENSOR"  /* senzor efectiv    */
           ;
```

Arch
====

```
Beam {
    (Point3D) firstPoint
    (Point3D) secondPoint
}

Sensor {
    (enum) type { CENTRAL, ROUTER, SENSOR }
    (bool) isSelected
    (Point3D) location
}

AnimationData {
    (bool) animating(Sensor)
    (Beam) connections[]
    (Sensor*) animatedSensors[]
}

Point3D {
    (float) x
    (float) y
    (float) z
}

Drawing {
    (enum) Color { CYAN, SALMON, YELLOW, LIME, WHITE }
    (void) SetColor(Color) // set pen color
    (void) MoveTo(Point3D) // position the pen
    (void) LineTo(Point3D) // draw a line
    (void) Cube(float)     // draw a cube with a given edge length
}

(void) render(beams[], sensors[], animationData) {
    foreach beam in beams {
        (void) Drawing.SetColor(WHITE)
        (void) Drawing.MoveTo((Point3D) beam.firstPoint)
        (void) Drawing.LineTo((Point3D) beam.secondPoint)
    }
    foreach sensor in sensors {
        MoveTo((Point3D) sensor.location)
        if (bool) sensor.isSelected or (bool) animationData.animating(sensor) {
            (void) Drawing.SetColor(LIME)
        } else {
            switch sensor.type  {
            case "CENTRAL":
                (void) Drawing.SetColor(CYAN)
                break
            case "ROUTER":
                (void) Drawing.SetColor(SALMON)
                break
            case "SENSOR":
                (void) Drawing.SetColor(YELLOW)
                break
            }
        }
        Drawing.Cube(1.0f)
    }
    foreach connection in animationData.connections {
        (void) Drawing.MoveTo((Point3D) connection.firstPoint)
        (void) Drawing.LineTo((Point3D) connection.secondPoint)
    }
}

Path {
    (Edge) activeEdges[]
}

Params {
    (Path) path[int]
}

(void) loop(beams[], sensors[], params) {
    (AnimationData) animationData
    foreach edge in params.path[(int) params.frame].activeEdges {
        (void) animationData.animatedSensors.push(edge.firstNode)
        (void) animationData.animatedSensors.push(edge.secondNode)
        (Beam) beam {
            firstPoint => (Point3D) edge.firstNode.location
            secondPoint => (Point3D) edge.secondNode.location
        }
        (void) animationData.connection.push(beam)
    }
    render(beams, sensors, animationData)
}

_generic_2d {
    (Point2D) location
    (union) { (Button), (Sensor) }
}

(void) onclick (p2d) {
    (_generic_2d) obj = ProjectSensorsOnCameraPlane.intersect(p2d)
    if obj is Sensor {
        (Sensor) sensor = obj.Sensor
        if sensor {
            (bool) sensor.isSelected = TRUE
            SetStartingSensor(sensor)
        } else {
            (void) deselectAll()
        }
    }
    if obj is Button {
        (void) obj.Button.Clicked()
    }
}

(void) PlayButton.Clicked() {
    (Params) params {
        (Beam[]) path[] => ComputePath(StartingSensor)
    }
}

ComputePath(S) {
    from SensorDistanceMap select sensor in S.range {
        switch sensor.type {
        case "SENSOR":
            SensorDistanceMap[S, sensor] = INFINITY
            break
        case "ROUTER":
            yield .push(Edge(S, sensor))
            SensorDistanceMap[S, sensor] = INFINITY
            defer ComputePath(sensor)
            break
        case "CENTRAL":
            yield .push(Edge(S, sensor))
            SensorDistanceMap[S, sensor] = INFINITY
            break
        }
    }
    yield .push(Edge[])
} yield Path[int]
```
