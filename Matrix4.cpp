

#include <iostream>
#include <iomanip>
#include "Matrix4.h"
#include <math.h>

// constructor - default to the zero matrix
Matrix4::Matrix4()
    { // default constructor
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            coordinates[row][col] = 0.0;
    } // default constructor

// equality operator
bool Matrix4::operator ==(const Matrix4 &other) const
    { // operator ==()
    // loop through, testing for mismatches
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            if (coordinates[row][col] != other.coordinates[row][col])
                return false;
    // if no mismatches, matrices are the same
    return true;
    } // operator ==()


// indexing - retrieves the beginning of a line
// array indexing will then retrieve an element
float * Matrix4::operator [](const int rowIndex)
    { // operator *()
    // return the corresponding row
    return coordinates[rowIndex];
    } // operator *()

// similar routine for const pointers
const float * Matrix4::operator [](const int rowIndex) const
    { // operator *()
    // return the corresponding row
    return coordinates[rowIndex];
    } // operator *()

// scalar operations
// multiplication operator (no division operator)
Matrix4 Matrix4::operator *(float factor) const
    { // operator *()
    // start with a zero matrix
    Matrix4 returnMatrix;
    // multiply by the factor
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            returnMatrix.coordinates[row][col] = coordinates[row][col] * factor;
    // and return it
    return returnMatrix;
    } // operator *()

// vector operations on homogeneous coordinates
// multiplication is the only operator we use
Homogeneous4 Matrix4::operator *(const Homogeneous4 &vector) const
    { // operator *()
    // get a zero-initialised vector
    Homogeneous4 productVector;
    
    // now loop, adding products
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            productVector[row] += coordinates[row][col] * vector[col];
    
    // return the result
    return productVector;
    } // operator *()


// and on Cartesian coordinates
Cartesian3 Matrix4::operator *(const Cartesian3 &vector) const
    { // cartesian multiplication
    // convert to Homogeneous coords and multiply
    Homogeneous4 productVector = (*this) * Homogeneous4(vector);

    // then divide back through
    return productVector.Point();
    } // cartesian multiplication

// matrix operations
// addition operator
Matrix4 Matrix4::operator +(const Matrix4 &other) const
    { // operator +()
    // start with a zero matrix
    Matrix4 sumMatrix;
    
    // now loop, adding products
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            sumMatrix.coordinates[row][col] = coordinates[row][col] + other.coordinates[row][col];

    // return the result
    return sumMatrix;
    } // operator +()

// subtraction operator
Matrix4 Matrix4::operator -(const Matrix4 &other) const
    { // operator -()
    // start with a zero matrix
    Matrix4 differenceMatrix;
    
    // now loop, adding products
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            differenceMatrix.coordinates[row][col] = coordinates[row][col] + other.coordinates[row][col];

    // return the result
    return differenceMatrix;
    } // operator -()

// multiplication operator
Matrix4 Matrix4::operator *(const Matrix4 &other) const
    { // operator *()
    // start with a zero matrix
    Matrix4 productMatrix;
    
    // now loop, adding products
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            for (int entry = 0; entry < 4; entry++)
                productMatrix.coordinates[row][col] += coordinates[row][entry] * other.coordinates[entry][col];

    // return the result
    return productMatrix;
    } // operator *()

// matrix transpose
Matrix4 Matrix4::transpose() const
    { // transpose()
    // start with a zero matrix
    Matrix4 transposeMatrix;
    
    // now loop, adding products
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            transposeMatrix.coordinates[row][col] = coordinates[col][row];

    // return the result
    return transposeMatrix;
    } // transpose()

// returns a column-major array of 16 values
// for use with OpenGL
columnMajorMatrix Matrix4::columnMajor() const
    { // columnMajor()
    // start off with an unitialised array
    columnMajorMatrix returnArray;
    // loop to fill in
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            returnArray.coordinates[4 * col + row] = coordinates[row][col];
    // now return the array
    return returnArray;
    } // columnMajor()

// routine that returns a row vector as a Homogeneous4
Homogeneous4 Matrix4::row(int rowNum)
	{ // row()
	// temporary variable
	Homogeneous4 returnValue;
	// loop to copy
	for (int column = 0; column < 4; column++)
		returnValue[column] = (*this)[rowNum][column];
	// and return it
	return returnValue;	
	} // row()

// and similar for a column
Homogeneous4 Matrix4::column(int colNum)
	{ // column()
	// temporary variable
	Homogeneous4 returnValue;
	// loop to copy
	for (int row = 0; row < 4; row++)
		returnValue[row] = (*this)[row][colNum];
	// and return it
	return returnValue;	
	} // column()

// static member functions that create specific matrices
// the zero matrix
Matrix4 Matrix4::Zero()
    { // Zero()
    // create a temporary matrix - constructor will automatically zero it
    Matrix4 returnMatrix;
	// so we just return it
	return returnMatrix;
    } // Zero()

// the identity matrix
Matrix4 Matrix4::Identity()
    { // Identity()
    // create a temporary matrix - constructor will automatically zero it
    Matrix4 returnMatrix;
    // fill in the diagonal with 1's
    for (int row = 0; row < 4; row++)
            returnMatrix.coordinates[row][row] = 1.0;

	// return it
	return returnMatrix;
	} // Identity()

Matrix4 Matrix4::Translate(const Cartesian3 &vector)
    { // Translation()
    // create a temporary matrix  and set to identity
    Matrix4 returnMatrix = Identity();

    // put the translation in the w column
    for (int entry = 0; entry < 3; entry++)
        returnMatrix.coordinates[entry][3] = vector[entry];

    // return it
    return returnMatrix;
    } // Translation()

 Matrix4 Matrix4::RotateX(float degrees)
 	{ // RotateX()
	// convert angle from degrees to radians
 	float theta = DEG2RAD(degrees);

    // create a temporary matrix  and set to identity
    Matrix4 returnMatrix = Identity();

	// now set the four coefficients affected
	returnMatrix.coordinates[1][1] = cos(theta);
	returnMatrix.coordinates[1][2] = sin(theta); //col,row
	returnMatrix.coordinates[2][1] = -sin(theta);
	returnMatrix.coordinates[2][2] = cos(theta);

	// return it
	return returnMatrix;
 	} // RotateX()
 	
 Matrix4 Matrix4::RotateY(float degrees)
 	{ // RotateY()
	// convert angle from degrees to radians
 	float theta = DEG2RAD(degrees);

    // create a temporary matrix  and set to identity
    Matrix4 returnMatrix = Identity();
    // -4.371e-08 
	// now set the four coefficients affected
	returnMatrix.coordinates[0][0] = cos(theta);
	returnMatrix.coordinates[0][2] = -sin(theta);
	returnMatrix.coordinates[2][0] = sin(theta);
	returnMatrix.coordinates[2][2] = cos(theta);

	// return it
	return returnMatrix;
 	} // RotateY()

 Matrix4 Matrix4::RotateZ(float degrees)
 	{ // RotateZ()
	// convert angle from degrees to radians
 	float theta = DEG2RAD(degrees);

    // create a temporary matrix  and set to identity
    Matrix4 returnMatrix = Identity();

	// now set the four coefficients affected
	returnMatrix.coordinates[0][0] = cos(theta);
	returnMatrix.coordinates[0][1] = sin(theta);
	returnMatrix.coordinates[1][0] = -sin(theta);
	returnMatrix.coordinates[1][1] = cos(theta);

	// return it
	return returnMatrix;
 	} // RotateZ()

 Matrix4 Matrix4::GetRotation(const Cartesian3& vector1, const Cartesian3& vector2)
 {
     Cartesian3 c = vector1.cross(vector2).unit();
     float cos = vector1.unit().dot(vector2.unit());
     float sin = sqrt(1 - pow(cos, 2));
     Matrix4 rot = Matrix4::Identity();
     rot.coordinates[0][0] = cos + (1 - cos) * pow(c.x, 2);
     rot.coordinates[0][1] = (1 - cos) * c.x * c.y - sin * c.z;
     rot.coordinates[0][2] = (1 - cos) * c.x * c.z + sin * c.y;
     rot.coordinates[1][0] = (1 - cos) * c.y * c.x + sin * c.z;
     rot.coordinates[1][1] = cos + (1 - cos) * pow(c.y, 2);
     rot.coordinates[1][2] = (1 - cos) * c.y * c.z - sin * c.x;
     rot.coordinates[2][0] = (1 - cos) * c.z * c.x - sin * c.y;
     rot.coordinates[2][1] = (1 - cos) * c.z * c.y + sin * c.x;
     rot.coordinates[2][2] = cos + (1 - cos) * pow(c.z, 2);
     return rot;
 }
 
// scalar operations
// additional scalar multiplication operator
Matrix4 operator *(float factor, const Matrix4 &matrix)
    { // operator *()
    // since this is commutative, call the other version
    return matrix * factor;
    } // operator *()

// stream input
std::istream & operator >> (std::istream &inStream, Matrix4 &matrix)
    { // operator >>()
    // just loop, reading them in
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            inStream >> matrix.coordinates[row][col];   
    // and return the stream
    return inStream;
    } // operator >>()

// stream output
std::ostream & operator << (std::ostream &outStream, const Matrix4 &matrix)
    { // operator <<()
    // just loop, reading them in
    for (int row = 0; row < 4; row++)
        for (int col = 0; col < 4; col++)
            outStream << std::setw(12) << std::setprecision(5) << std::fixed << matrix.coordinates[row][col] << ((col == 3) ? "\n" : " "); 
    // and return the stream
    return outStream;
    } // operator <<()

Matrix4 Matrix4::Scale(float x, float y, float z)
{
    Matrix4 returnMatrix = Identity();

    returnMatrix[0][0] = x;
    returnMatrix[1][1] = y;
    returnMatrix[2][2] = z;

    return returnMatrix;
}
    
Matrix4 Matrix4::RotateDirection(const Cartesian3& direction, const Cartesian3& up)
{
    Cartesian3 xaxis = up.cross(direction).unit(); // calculate x using cross product of up and direction
    Cartesian3 yaxis = direction.cross(xaxis).unit(); // calculate y using cross product of direction and x

    // Set the vectors in the matrix
    Matrix4 out;
    out.coordinates[0][0] = xaxis.x;
    out.coordinates[0][1] = yaxis.x;
    out.coordinates[0][2] = direction.x;
    out.coordinates[0][3] = 0.f;

    out.coordinates[1][0] = xaxis.y;
    out.coordinates[1][1] = yaxis.y;
    out.coordinates[1][2] = direction.y;
    out.coordinates[1][3] = 0.f;

    out.coordinates[2][0] = xaxis.z;
    out.coordinates[2][1] = yaxis.z;
    out.coordinates[2][2] = direction.z;
    out.coordinates[2][3] = 0.f;

    out.coordinates[3][0] = 0.f;
    out.coordinates[3][1] = 0.f;
    out.coordinates[3][2] = 0.f;
    out.coordinates[3][3] = 1.f;

    return out;
}
// Function to construct a view matrix
Matrix4 Matrix4::ViewMatrix(const Cartesian3& camerpos, const Cartesian3& target, const Cartesian3& up)
{
    Cartesian3 forward, Up, right;

    forward = camerpos - target;
    forward.unit();

    right = up.cross(forward);
    right.unit();

    Up = forward.cross(right);
    Up.unit();

    auto rotationMatrix = Matrix4::Identity();
    // col, row
    rotationMatrix.coordinates[0][0] = right.x;
    rotationMatrix.coordinates[1][0] = Up.x;
    rotationMatrix.coordinates[2][0] = forward.x;
    rotationMatrix.coordinates[3][0] = 0.0f;

    rotationMatrix.coordinates[0][1] = right.y;
    rotationMatrix.coordinates[1][1] = Up.y;
    rotationMatrix.coordinates[2][1] = forward.y;
    rotationMatrix.coordinates[3][1] = 0.0f;

    rotationMatrix.coordinates[0][2] = right.z;
    rotationMatrix.coordinates[1][2] = Up.z;
    rotationMatrix.coordinates[2][2] = forward.z;
    rotationMatrix.coordinates[3][2] = 0.0f;

    rotationMatrix.coordinates[0][3] = 0.0f;
    rotationMatrix.coordinates[1][3] = 0.0f;
    rotationMatrix.coordinates[2][3] = 0.0f;
    rotationMatrix.coordinates[3][3] = 1.0f;

    Matrix4 translation = Matrix4::Identity();

    translation.coordinates[0][0] = 1.0f;
    translation.coordinates[1][0] = 0.0f;
    translation.coordinates[2][0] = 0.0f;
    translation.coordinates[3][0] = 0.0f;

    translation.coordinates[0][1] = 0.0f;
    translation.coordinates[1][1] = 1.0f;
    translation.coordinates[2][1] = 0.0f;
    translation.coordinates[3][1] = 0.0f;

    translation.coordinates[0][2] = 0.0f;
    translation.coordinates[1][2] = 0.0f;
    translation.coordinates[2][2] = 1.0f;
    translation.coordinates[3][2] = 0.0f;

    translation.coordinates[0][3] = -camerpos.x;
    translation.coordinates[1][3] = -camerpos.y;
    translation.coordinates[2][3] = -camerpos.z;
    translation.coordinates[3][3] = 1.0f;
    
    return rotationMatrix * translation;
}

// Construct a look matrix
Matrix4 Matrix4::Look(const Cartesian3& position, const Cartesian3& target, const Cartesian3& up)
{	
    Cartesian3 forward, Up, right;
    Cartesian3 x, y, z;

    forward = target - position;
    forward.unit();

    right = up.cross(forward);
    right.unit();

    Up = forward.cross(right);
    Up.unit();

    z = forward;
    y = Up;
    x = right;

    z = z.unit();
    y = y.unit();
    x = x.unit();

    // Set up rotation as a row major matrix
    Matrix4 rotation; // col, row 
    // x
    rotation.coordinates[0][0] = x.x;
    rotation.coordinates[1][0] = y.x;
    rotation.coordinates[2][0] = z.x;
    rotation.coordinates[3][0] = 0.0f;

    // y
    rotation.coordinates[0][1] = x.y;
    rotation.coordinates[1][1] = y.y;
    rotation.coordinates[2][1] = z.y;
    rotation.coordinates[3][1] = 0.0f;

    // z
    rotation.coordinates[0][2] = x.z;
    rotation.coordinates[1][2] = y.z;
    rotation.coordinates[2][2] = z.z;
    rotation.coordinates[3][2] = 0.0f;

    rotation.coordinates[0][3] = 0.0f;
    rotation.coordinates[1][3] = 0.0f;
    rotation.coordinates[2][3] = 0.0f;
    rotation.coordinates[3][3] = 1.0f;


    Matrix4 translation;
    translation.coordinates[0][0] = 1.0f;
    translation.coordinates[1][0] = 0.0f;
    translation.coordinates[2][0] = 0.0f;
    translation.coordinates[3][0] = 0.0f;

    translation.coordinates[0][1] = 0.0f;
    translation.coordinates[1][1] = 1.0f;
    translation.coordinates[2][1] = 0.0f;
    translation.coordinates[3][1] = 0.0f;

    translation.coordinates[0][2] = 0.0f;
    translation.coordinates[1][2] = 0.0f;
    translation.coordinates[2][2] = 1.0f;
    translation.coordinates[3][2] = 0.0f;

    translation.coordinates[0][3] = 0.0f;
    translation.coordinates[1][3] = 0.0f;
    translation.coordinates[2][3] = 0.0f;
    translation.coordinates[3][3] = 1.0f;
    
    return rotation * translation;
}


