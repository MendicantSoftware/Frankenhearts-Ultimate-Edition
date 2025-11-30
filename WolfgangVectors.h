#pragma once

namespace WG {
    
    /**
    * Stores a point in 2D space
    */
    class Vector {
        public:

        double dx, dy;

        Vector() {
            dx = -50;
            dy = -50;
        }

        Vector(double dXPosition, double dYPosition) {
            dx = dXPosition;
            dy = dYPosition;
        } 

    };

    /**
    * Takes two WG::Vector objects, defining an edge
    */
    class Edge {

        public:

        Vector wvV1;
        Vector wvV2;

        Edge() {

        }

        Edge(Vector wvFirstVector, Vector mvConnectingVector) {
            wvV1 = wvFirstVector;
            wvV2 = mvConnectingVector;
        }

    };

}