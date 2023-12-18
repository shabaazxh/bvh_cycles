
#ifndef _BVHDATA_H
#define _BVHDATA_H

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include <vector>
#include <string>
#include <sstream>
#include "Cartesian3.h"
#include "Matrix4.h"
#include <fstream>
#include <map>
#include <math.h>
#include "Quaternion.h"
#include <queue>

// A class for each joint
class Joint
	{ // class Joint
	public:
	// joint id
	int id;
	// joint name
	std::string joint_name;
	// joint offset
	float joint_offset[3] = {0.0f, 0.0f, 0.0f};
	// joint channel
	std::vector<std::string> joint_channel;
	// joint's children
	std::vector<Joint> Children;
	}; // class Joint

// bvh data class
class BVHData
	{ // class BVHData
	public:

	// the root joint of armature
	Joint root;

	// bvh frame count
	int frame_count;

	// frame rate of the animation
	float frame_time;

	// a vector to store all bones' name
	std::vector<std::string> Bones;

	// a vector to store all joints
	std::vector<Joint*> all_joints;

	// a vector to store the parent bone's id for each joint
	std::vector<int> parentBones;

	// a vector to store all frames of the animation
	// this is *JUST* a huge 2D array of floats
	// in each frame, we have six channels for position and rotation for each joint
	// listed in strict numerical order
	// Stores position data for only hip joint
	std::vector<std::vector<float>> frames;

	// a vector to store all bones' offsets
	std::vector<Cartesian3> boneTranslations;

	// a vector to store all bones' rotations for each frame
	std::vector<std::vector<Cartesian3>> boneRotations;

	Quaternion otherQuat;

	Quaternion combinedQuaternion;

	Quaternion GetRotation() const { return combinedQuaternion; }

	//std::queue<std::pair<Quaternion, Matrix4>> transitionTo;
	std::vector<BVHData> transitionTo;
	
private:
	// id for each channel
	std::map<std::string, int> BVH_CHANNEL =
		{ // BVH_CHANNEL
			{"Xposition", 0},
			{"Yposition", 1},
			{"Zposition", 2},
			{"Xrotation", 3},
			{"Yrotation", 4},
			{"Zrotation", 5}
		}; // BVH_CHANNEL

public:
	// constructor
	BVHData();

	// render bvh animation by given a sequence of frames data
	void Render(Matrix4& viewMatrix, float scale, int frame, const Quaternion& blendBetween = Quaternion(), bool useQuat = false);

	// render a single joint by given frame id
	void RenderJoint(Matrix4& viewMatrix, Matrix4 HierarchicalMatrix, Joint* joint, float scale, int frame);

	// render cylinder given the start position and the end position
	void RenderCylinder(Matrix4& viewMatrix, Cartesian3 start, Cartesian3 end, const Matrix4& a, const std::string& name);

	// render a single cylinder given radius, length and vertical slices
	void Cylinder(Matrix4& viewMatrix, float radius, float halfLength, int slices);

	// get all joints in a sequence by searching the tree structure and store it into this class
	void GetAllJoints(Joint&, std::vector<Joint*>&);

	// Routines for file I/O
	// read data from bvh file
	bool ReadFileBVH(const char* fileName);

	// read a single line and tokenise it
	void NewLine(std::ifstream&, std::vector<std::string>&);

	// split string with the given key character
	void StringSplit(std::string, std::vector<std::string>&);

	// recursive descent parser for the hierarchy
	void ReadHierarchy(std::ifstream&, std::vector<std::string>&, Joint&, int parent);

	// read motion(frames) from file
	void ReadMotion(std::ifstream&);

	// load all rotation and translation data into this class
	void loadAllData(std::vector<std::vector<Cartesian3>>& rotations, std::vector<Cartesian3>& translations, std::vector<std::vector<float>>& frames);

	// load all rotation data into this class
	void loadRotationData(std::vector<Cartesian3> &rotations, std::vector<float>& frames);

	// check whether the given string is a number
	bool isNumeric(const std::string&);

	bool useQuat;

	//std::queue<BVHData> transitionTo;
};

inline void drawMatrix(const Matrix4& matrix, const Matrix4& viewMatrix, Cartesian3 vs)
{

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	auto modelview = matrix;
    // Apply the matrix
	glLineWidth(5.0f);

	auto xAxis = Cartesian3(modelview[0][0], modelview[1][0], modelview[2][0]);
	xAxis = xAxis;
	xAxis = xAxis.unit();
	xAxis = 0.01 * xAxis;

    glBegin(GL_LINES);
	const GLfloat red[4] = {1.0, 0.0, 0.0, 1.0};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);
    // Draw X axis in red
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex3f(vs.x, vs.y, vs.z);
    glVertex3f(xAxis.x, xAxis.y, xAxis.z);

	auto yAxis = Cartesian3(modelview[0][1], modelview[1][1], modelview[2][1]);
	yAxis = yAxis;
	yAxis = yAxis.unit();
	yAxis = 0.01 * yAxis;
    // Draw Y axis in green
	const GLfloat green[4] = {0.0, 1.0, 0.0, 1.0};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex3f(vs.x, vs.y, vs.z);
    glVertex3f(yAxis.x, yAxis.y, yAxis.z);

    // Draw Z axis in blue
	const GLfloat blue[4] = {0.0, 0.0, 1.0, 1.0};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
	auto zAxis = Cartesian3(modelview[0][2], modelview[1][2], modelview[2][2]);
	zAxis = zAxis;
	zAxis = zAxis.unit();
	zAxis = 0.01 * zAxis;
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex3f(vs.x, vs.y, vs.z);
    glVertex3f(zAxis.x, zAxis.y, zAxis.z);

    glEnd();
}

#endif
