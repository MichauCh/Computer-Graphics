var MATRIX = {
   degToRad: function(angle) {
      return (angle*Math.PI/180);
   },
   getProjection: function(angle, a, zMin, zMax) {
      var tan = Math.tan(MATRIX.degToRad(0.5*angle)),
         A=-(zMax+zMin)/(zMax-zMin),
            B=(-2*zMax*zMin)/(zMax-zMin);

      return [
          .5/tan,          0,   0,    0,
               0,   .5*a/tan,   0,    0,
               0,          0,   A,   -1,
               0,          0,   B,    0
      ]
   },
   getIdentityMatrix: function () {
      return [
         1, 0, 0, 0,
         0, 1, 0, 0,
         0, 0, 1, 0,
         0, 0, 0, 1
      ];
   },
   // rotate movement matrix with angle around X axis
   rotateX: function(movMat, angle) {
      var sin = Math.sin(angle);
      var cos = Math.cos(angle);
      var matElem1 = movMat[1],
          matElem5 = movMat[5],
          matElem9 = movMat[9];

      movMat[1] = movMat[1]*cos - movMat[2]*sin;
      movMat[5] = movMat[5]*cos - movMat[6]*sin;
      movMat[9] = movMat[9]*cos - movMat[10]*sin;

      movMat[2] = movMat[2]*cos + matElem1*sin;
      movMat[6] = movMat[6]*cos + matElem5*sin;
      movMat[10] = movMat[10]*cos + matElem9*sin;
   },

   // rotate movement matrix with angle around Y axis
   rotateY: function(movMat, angle) {
      var sin = Math.sin(angle);
      var cos = Math.cos(angle);
      var matElem0 = movMat[0],
          matElem4 = movMat[4],
          matElem8 = movMat[8];

      movMat[0] = movMat[0]*cos + movMat[2]*sin;
      movMat[4] = movMat[4]*cos + movMat[6]*sin;
      movMat[8] = movMat[8]*cos + movMat[10]*sin;

      movMat[2] = movMat[2]*cos - matElem0*sin;
      movMat[6] = movMat[6]*cos - matElem4*sin;
      movMat[10] = movMat[10]*cos - matElem8*sin;
   },

   // rotate movement matrix with angle around Z axis
   rotateZ: function(movMat, angle) {
      var sin = Math.sin(angle);
      var cos = Math.cos(angle);
      var matElem0 = movMat[0],
          matElem4 = movMat[4],
          matElem8 = movMat[8];

      movMat[0] = movMat[0]*cos - movMat[1]*sin;
      movMat[4] = movMat[4]*cos - movMat[5]*sin;
      movMat[8] = movMat[8]*cos - movMat[9]*sin;

      movMat[1] = movMat[1]*cos + matElem0*sin;
      movMat[5] = movMat[5]*cos + matElem4*sin;
      movMat[9] = movMat[9]*cos + matElem8*sin;
   },
   // translate movement matrix by trans along Z axis
   translateZ: function (movMat, trans) {
      movMat[14] += trans;
   }
};