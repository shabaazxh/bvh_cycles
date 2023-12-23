#include "BVHData.h"

// constructor
BVHData::BVHData()
	{ // constructor
	isTransitioningBack = true;
	m_AnimState = Running;
	m_currentState = Running;
	} // constructor
	
// read data from bvh file
// a basic recursive-descent parser
bool BVHData::ReadFileBVH(const char* fileName)
	{ // ReadFileBVH()
	// open a file stream and check validity
	std::ifstream inFile(fileName);
	if (inFile.bad())
		return false;

	// temporary storage to read in a single line
	std::string line;
	// a vector of the tokens on the line
	std::vector<std::string> tokens;
	
	// loop through the file one line at a time
	while (std::getline(inFile, line))
		{ // more lines in the file
		// not an empty line
		if (line.size() != 0)
			{ // non-empty line
			// chop the line into tokens
			StringSplit(line, tokens);

			// if the first token is HIERARCHY, it is the logical structure of the character
			if (tokens[0] == "HIERARCHY")
				{ // hierarchy
				// read in a line and split it into tokens
				NewLine(inFile, tokens);
				// create a joint object 
				Joint joint;
				// read in the hierarchy based at the joint
				ReadHierarchy(inFile, tokens, joint, -1);
				// and set it to be the root
				this->root = joint;
				} // hierarchy
			// otherwise, if the first token is MOTION, it is the animation data
			else if (tokens[0] == "MOTION")
				{ // motion
				ReadMotion(inFile);
				break;
				} // motion
			else
				{ // otherwise
				// ignore everything els
				} // otherwise
			} // non-empty line
		} // more lines in the file

	// do a recursive search to set up the vector of pointers to the joints
	GetAllJoints(this->root, this->all_joints);
	// load all rotation and translation data into this class
	loadAllData(this->boneRotations, this->boneTranslations, this->frames);
	return true;
	} // ReadFileBVH()

// read a single line and tokenise it
void BVHData::NewLine(std::ifstream& inFile, std::vector<std::string>& tokens)
	{ // NewLine()
	// the next line
	std::string line;
	// empty out the array of tokens
	tokens.clear();
	// read in the line
	std::getline(inFile, line);
	// and split it into tokens
	StringSplit(line, tokens);
	} // NewLine()

// split string with the given key character
void BVHData::StringSplit(std::string input, std::vector<std::string>& tokens)
	{ // StringSplit()
	// empty out the array of tokens
	tokens.clear();
	// create a local string stream for tokenising
	std::istringstream iss(input);
	while (!iss.eof()) 
		{ // more tokens to come
		// create a token
		std::string token;
		// read it in
		iss >> token;
		// and add it to the vector
		tokens.push_back(token);
		} // more tokens to come
	} // StringSplit()

// check whether the given string is a number
bool BVHData::isNumeric(const std::string& str) 
	{ // isNumeric()
	// loop through the characters in the string
	for (char c : str) 
		// if it's not a digit, return early
		if (!std::isdigit(c)) 
			return false;
	// return success
	return true;
	} // isNumeric()

// recursive descent parser for the hierarchy
void BVHData::ReadHierarchy(std::ifstream& inFile, std::vector<std::string>& line, Joint& joint, int parent)
	{ // ReadHierarchy()
	// the new joint will have the next available ID 
	// (0 offset means that it will be equal to the existing size)
	joint.id = this->Bones.size();
	// the second token (#1) will be the name of the joint
	joint.joint_name = line[1];
	// add the name to the list
	this->Bones.push_back(joint.joint_name);
	// and set the parent bone
	this->parentBones.push_back(parent);
	// this seems redundant, but removing it crashes. This works. For now.
	int id = this->parentBones.size() - 1;
	// read in the next line & tokenise it
	NewLine(inFile, line);
	// if the beginning of the line is a left brace, we're starting a group of children
	if (line[0] == "{")
		{ // group of children
		// ignore the rest of the line and read in a new one
		NewLine(inFile, line);
		while (line[0] != "}")
			{ // until we hit the close of the group
			// The first token tells us which type of line
			// OFFSET is the offset from the parent
			if (line[0] == "OFFSET")
				{
				joint.joint_offset[0] = std::stof(line[1]);
				joint.joint_offset[1] = std::stof(line[2]);
				joint.joint_offset[2] = std::stof(line[3]);
				}
			// CHANNELS defines how many floats are needed for the animation, and which ones
			else if (line[0] == "CHANNELS")
				{ // channel information
				for (int i = 0; i < std::stoi(line[1]); i++)
					joint.joint_channel.push_back(line[i + 2]);
				} // channel information
			// JOINT defines a new joint
			else if (line[0] == "JOINT")
				{ // joint information
				Joint child;
				ReadHierarchy(inFile, line, child, id);
				joint.Children.push_back(child);
				} // joint information 
			// At the leaf of the hierarchy, there is no joint. Instead it says End 
			else if (line[0] == "End")
				{ // end site
				// read in and ignore three extra lines
				for (int i = 0; i < 3; i++) 
					NewLine(inFile, line);
				} // end site
			// always read the next line when done processing this line
			NewLine(inFile, line);
			} // until we hit the close of the group
		} // group of children
	} // ReadHierarchy()

// read motion(frames) from file
void BVHData::ReadMotion(std::ifstream& inFile)
	{ // ReadMotion()
	// a single line from the file
	std::string line;
	// and the tokens it breaks into
	std::vector<std::string> tokens;
	// the next line should specify how many frames, so read it in
	NewLine(inFile, tokens);
	// convert to an integer and save
	this->frame_count = std::stoi(tokens[1]);
	// the next line should specify how many seconds per frame, so read it in
	NewLine(inFile, tokens);
	// and convert it to a float
	this->frame_time = std::stof(tokens[2]);
	// after that, we loop until the end of the file
	while (std::getline(inFile, line))
		{ // more data
		// ignore empty lines
		if (line.size() != 0)
			{ // non-empty line
			// split the line into tokens
			StringSplit(line, tokens);
			// create a vector to store all the animation data for the frame
			std::vector<float> frame;
			// loop through the line, one token at a time
			for (size_t i = 0; i < tokens.size(); i++)
				{ // per token
				if (!isNumeric(tokens[i]))
					{ // if it's not numeric
					frame.push_back(std::stof(tokens[i]));
					} // if it's not numeric
				} // per token
			// store frame into this class
			this->frames.push_back(frame);
			}
		} // more data
	} // ReadMotion()

// Negate the rotations in the rotations array
void BVHData::NegateRotations()
{
	for(int i = 0; i < frame_count; i++)
	{
		for(int j = 0; j < all_joints.size(); j++)
		{
			boneRotations[i][j] = -boneRotations[i][j];
		}
	}
}

std::pair<Quaternion, Cartesian3> BVHData::BlendPose(Cartesian3& a, Cartesian3& b, double time, float slerpAmount, Cartesian3& currentPos, Cartesian3& other)
{
	// Animation A rotation
	Quaternion a_rotX = Quaternion(a.x, Cartesian3(1.0f, 0.0f, 0.0f).unit()); a_rotX.Normalize();
	Quaternion a_rotY = Quaternion(a.y, Cartesian3(0.0f, 1.0f, 0.0f).unit()); a_rotY.Normalize();
	Quaternion a_rotZ = Quaternion(a.z, Cartesian3(0.0f, 0.0f, 1.0f).unit()); a_rotZ.Normalize();
	Quaternion a_animARotQuat = (a_rotZ * a_rotY) * a_rotX;

	// Animation B rotation
	Quaternion b_rotX = Quaternion(b.x, Cartesian3(1.0f, 0.0f, 0.0f).unit()); b_rotX.Normalize();
	Quaternion b_rotY = Quaternion(b.y, Cartesian3(0.0f, 1.0f, 0.0f).unit()); b_rotY.Normalize();
	Quaternion b_rotZ = Quaternion(b.z, Cartesian3(0.0f, 0.0f, 1.0f).unit()); b_rotZ.Normalize();
	Quaternion b_animARotQuat = (b_rotZ * b_rotY) * b_rotX;

	// Check how we're going to lerp
	double amount_to_lerp = isTransitioningBack ? 1.0 - (time / slerpAmount) : time / slerpAmount;

	if(isTransitioningBack)
	{
		if(amount_to_lerp <= 0.0)
		{
			amount_to_lerp = 0.0f;
			m_AnimState = Running;
			m_currentState = Running;
			isTransitioningBack = false;	
			transitionTo.clear();
		}
	} else 
	{
		if(amount_to_lerp > 1.0)
		{
			amount_to_lerp = 1.0f;
		}
	}

	Quaternion blendedRot = Slerp(a_animARotQuat, b_animARotQuat, amount_to_lerp);
	Cartesian3 blendedPos = Lerp(currentPos, other, amount_to_lerp);
	
	return {blendedRot, blendedPos};
}

std::pair<Quaternion, Cartesian3> BVHData::CalculateNewPose(int frame, float time, float slerpAmount, int jointID)
{
	// Set up pose for animation A and B
	auto f = (frame + 1) % frame_count;
	Cartesian3 anim_pose_A = SampleAnimation(f, jointID);
	Cartesian3 anim_pose_B = SampleAnimation(f, jointID);
	// if there is a transition animation clip to transition to, set rotation to the animation clips rotation
	// at this current frame 
	Cartesian3 other;
	if(!transitionTo.empty())
	{
		// Get the first animation clip and sample animation to get current joint pose transforms
		BVHData transitionAnim = transitionTo[transitionTo.size() - 1];
		auto sampleFrame = (frame + 1) % transitionAnim.frame_count;
		anim_pose_B = transitionTo[transitionTo.size() - 1].SampleAnimation(sampleFrame, jointID);

		other = transitionTo[transitionTo.size() - 1].SamplePosition(sampleFrame, jointID);
		if(m_AnimState == TurnLeft || m_AnimState == TurnRight)
		{
			if(sampleFrame == (transitionAnim.frame_count - 1) && jointID == 64)
			{
				// isTransitioningBack = true;
				//transitionTo.clear();
			}
		}
	}

	Cartesian3 currentPosition = SampleAnimation(f, jointID);
	// Blend the poses
	std::pair<Quaternion, Cartesian3> blendedPose = BlendPose(anim_pose_A, anim_pose_B, time, slerpAmount, currentPosition, other);
	return blendedPose;
}

Cartesian3 BVHData::SampleAnimation(int frame, int jointID)
{
	return boneRotations[frame][jointID];
}

Cartesian3 BVHData::SamplePosition(int frame, int jointID)
{
	auto joint = all_joints[jointID];

	Cartesian3 position = Cartesian3(0.0f, 0.0f, 0.0f);
	for(int i = 0; i < joint->joint_channel.size(); i++)
    {
        const std::string& channel = joint->joint_channel[i];
        float value = frames[frame][BVH_CHANNEL[channel]];
		// if the channel is for x position, set the x position
        if(channel == "Xposition")
        {
			position.x = value;
        }
		// if the channel is for y position, set the y position
        if(channel == "Yposition")
        {
           position.y = value;
        }
		// if the channel is for z position, set the z position
        if(channel == "Zposition")
        {
            position.z = value;
        }
	}

	return position;
}

// render hierarchy for a given frame
void BVHData::Render(Matrix4& viewMatrix, float scale, int frame, double time,  Cartesian3& playerpos, Cartesian3& dir, Matrix4& playerTransform)
{ // Render()
	RenderJoint(viewMatrix, Matrix4::Identity(), &this->root, scale, frame, playerpos, dir, playerTransform);
} // Render()

static int cycles = 0;
// render a single joint for a given frame
void BVHData::RenderJoint(Matrix4& viewMatrix, Matrix4 parentMatrix, Joint* joint, float scale, int frame, Cartesian3& playerpos, Cartesian3& dir, Matrix4& playerTransform)
	{ // RenderJoint()

	// Time since animation started
	auto currentTime = std::chrono::high_resolution_clock::now();
	double nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(currentTime - timeStart).count();
	double time_in_seconds = nanoseconds / 1e+9;

	auto f = (frame + 1) % frame_count;

	// Determine updated pose for current joint
	std::pair<Quaternion, Cartesian3> updatedPose = CalculateNewPose(frame, time_in_seconds, 0.5f, joint->id);
	Matrix4 finalRotationMatrix = updatedPose.first.ToRotationMatrix();

	Homogeneous4 offset_from_parent = Homogeneous4(joint->joint_offset[0] * scale, joint->joint_offset[1] * scale, joint->joint_offset[2] * scale, 1.0f);

    if(joint->joint_name == "mixamorig1:Hips")
    {
        // offset_from_parent.x = playerpos.x + updatedPose.second.x;
        // offset_from_parent.z = playerpos.z + updatedPose.second.z;
        // when turn animation completes, its removed to playerpos becomes previous animations transform which is at 0,29,0     
        auto previous = playerpos;
        if(m_AnimState == TurnLeft || m_AnimState == TurnRight)
        {
            if(!transitionTo.empty())
            {
                auto BVH = transitionTo[transitionTo.size() - 1];
                if((BVH.frame_count - 1) == ((frame + 1) % BVH.frame_count))
                {
                    auto a = BVH.SampleAnimation((frame + 1) % BVH.frame_count, joint->id);
                    Quaternion a_rotY = Quaternion(a.y, Cartesian3(0.0f, 1.0f, 0.0f).unit()); 
        
                    //std::cout << "MOVE BACK: " << playerpos <<  " Rot: " << a.y << std::endl;
                    
                    // playerpos = Cartesian3(0,0,0);
                    std::cout << "player pos: " << playerpos << std::endl; 
                    dir.Rotate(a.y, Cartesian3(0.0f, 1.0f, 0.0f));
                    dir = dir.unit();   
                    // playerpos = offset_from_parent.Point();
                    // std::cout << "direction: " << dir << std::endl;                  
                }
            }
        }
        
    }


	auto Offset = Matrix4::Translate({offset_from_parent.x, offset_from_parent.y, offset_from_parent.z});

	auto global = Matrix4::Identity();
	global = parentMatrix * Offset * finalRotationMatrix;

	// multiply offset by parent matrix to get it into the correct space to render
	offset_from_parent = parentMatrix * offset_from_parent;

	// for each child of the current joint, recursively render the joint
	// using start and end position
	for(auto& child : joint->Children)
	{
        auto end = global * Homogeneous4(child.joint_offset[0] * scale, child.joint_offset[1]* scale, child.joint_offset[2]* scale, 1.0f);
		auto endpoint = end.Point();
		auto start = offset_from_parent.Point();
		RenderCylinder(viewMatrix, start, endpoint, finalRotationMatrix, joint->joint_name);
        // Recursively render the child joint
        RenderJoint(viewMatrix, global, &child, scale, frame, playerpos, dir, playerTransform);
	}

} // RenderJoint()

// render cylinder given the start position and the end position
void BVHData::RenderCylinder(Matrix4& viewMatrix, Cartesian3 start, Cartesian3 end, const Matrix4& a, const std::string& name)
	{ // RenderCylinder()

	// Calculate the difference between the two points
    Cartesian3 diff = end - start;

    // Calculate the length of the cylinder
    float length = diff.length();

	// Normalize the difference vector to get the direction
    Cartesian3 dir = diff.unit();
	const GLfloat boneCol[4] = {0.0, 0.0, 0.0, 1.0};

	// Render skeleton using lines
	if(false)
	{
		auto viewStart = viewMatrix * start;
		auto viewEnd = viewMatrix * end;
		glLineWidth(10.0f);
		glColor3f(1.0, 0.0f, 0.0f);
		glBegin(GL_LINES);
		glVertex3f(viewStart.x, viewStart.y, viewStart.z);
		glVertex3f(viewEnd.x, viewEnd.y, viewEnd.z);
		glEnd();
	}

	
	if(name == "mixamorig1:Hips")
	{
		auto vs = viewMatrix * start;
		//drawMatrix(a, viewMatrix, vs);
	}
	
	// set up the transformations for the cylinder
	auto cyTranslate = Matrix4::Translate({start.x, start.y, start.z});
	auto cyMatrix = viewMatrix * cyTranslate * Matrix4::RotateDirection(dir); 

	Cylinder(cyMatrix, 1.0, length, 10);

	} // RenderCylinder()

// render a single cylinder given radius, length and vertical slices
void BVHData::Cylinder(Matrix4& viewMatrix, float radius, float Length, int slices)
	{  // Cylinder()
	// start a set of triangles
	glBegin(GL_TRIANGLES);
	// loop through the given number of slices
	for (int i = 0; i < slices; i++) 
		{ // per slice
		// work out the angles around the main axis for the start and end of the slice
		float theta = (float)(i * 2.0f * M_PI / slices);
		float nextTheta = (float)((i + 1) * 2.0f * M_PI / slices);
		float midTheta = 0.5 * (theta + nextTheta);

		// the top vertex is always in the same place
		Homogeneous4 center_up = viewMatrix * Homogeneous4(0.0, 0.0, Length, 1);
		// we have two points on the upper circle of the cylinder
		Homogeneous4 c_edge1 = viewMatrix * Homogeneous4(radius * cos(theta), radius * sin(theta), Length, 1);
		Homogeneous4 c_edge2 = viewMatrix * Homogeneous4(radius * cos(nextTheta), radius * sin(nextTheta), Length, 1);
		// and two points on the bottom circle
		Homogeneous4 c_edge3 = viewMatrix * Homogeneous4(radius * cos(nextTheta), radius * sin(nextTheta), 0, 1);
		Homogeneous4 c_edge4 = viewMatrix * Homogeneous4(radius * cos(theta), radius * sin(theta), 0, 1);
		// and a point in the middle of the bottom
		Homogeneous4 center_bottom = viewMatrix * Homogeneous4(0.0, 0.0, 0, 1);

		// normal vectors are tricky because we need to AVOID using the translation
		// We can either use a triangle face normal, or we can do a hack ;-)
		// because we know that they are from the origin to given points

		// we have three normals: one for the top
		Cartesian3 normal_up = viewMatrix * Cartesian3(0, 0, 1.0) - viewMatrix * Cartesian3(0.0, 0.0, 0.0);
		// one for the middle
		Cartesian3 normal_edge = viewMatrix * Cartesian3(cos(midTheta), sin(midTheta), 0.0) - viewMatrix * Cartesian3(0.0, 0.0, 0.0);
		// and one for the bottom
		Cartesian3 normal_bottom = viewMatrix * Cartesian3(0, 0, -1.0) - viewMatrix * Cartesian3(0.0, 0.0, 0.0);

		// render the top triangle
		glNormal3fv(&normal_up.x);
		glVertex4fv(&center_up.x);
		glVertex4fv(&c_edge1.x);
		glVertex4fv(&c_edge2.x);

		// and the side triangles
		glNormal3fv(&normal_edge.x);
		glVertex4fv(&c_edge2.x);
		glVertex4fv(&c_edge1.x);
		glVertex4fv(&c_edge4.x);

		glNormal3fv(&normal_edge.x);
		glVertex4fv(&c_edge2.x);
		glVertex4fv(&c_edge4.x);
		glVertex4fv(&c_edge3.x);

		// and the bottom triangle
		glNormal3fv(&normal_bottom.x);
		glVertex4fv(&c_edge3.x);
		glVertex4fv(&c_edge4.x);
		glVertex4fv(&center_bottom.x);

		}
	glEnd();
	} // Cylinder()

// get all joints in a sequence by searching the tree structure and store it into this class
void BVHData::GetAllJoints(Joint& joint, std::vector<Joint*>& joint_list)
	{ // GetAllJoints()
	// it's recursive, so push the current one
	joint_list.push_back(&joint);
	// base case: end site, so return
	if (joint.Children.size() == 0)
		return;
	// otherwise, loop through the children
	for (size_t i = 0; i < joint.Children.size(); i++)
		// adding them recursively
		GetAllJoints(joint.Children[i], joint_list);
	} // GetAllJoints()

// load all rotation and translation data into this class
void BVHData::loadAllData(std::vector<std::vector<Cartesian3>>& rotations, std::vector<Cartesian3>& translations, std::vector<std::vector<float>>& frames)
	{ // loadAllData()
	// store all rotations
	for (size_t i = 0; i < frames.size(); i++)
		{ // per frame 
		std::vector<Cartesian3> frame_rotations;
		loadRotationData(frame_rotations, frames[i]);
		rotations.push_back(frame_rotations);
		} // per frame
	// store all offsets/translations
	for (size_t i = 0; i < this->all_joints.size(); i++)
		{ // per joint
		float x = this->all_joints[i]->joint_offset[0];
		float y = this->all_joints[i]->joint_offset[1];
		float z = this->all_joints[i]->joint_offset[2];
		translations.push_back(Cartesian3(x, y, z));
		} // per joint

	NegateRotations();

	} // loadAllData()

// load all rotation data into this class
void BVHData::loadRotationData(std::vector<Cartesian3>& rotations, std::vector<float>& frames)
	{ // loadRotationData()
	for (size_t j = 0, j_c = 0; j < frames.size(); j_c++)
		{ // per frame
		float rotation[3] = { 0, 0, 0 };
		for (size_t k = 0; k < this->all_joints[j_c]->joint_channel.size(); k++) // for each channel
			{ // per channel
			// if the channel includes "rotation" in the name
			if (this->all_joints[j_c]->joint_channel[k].substr(1) == "rotation")
				{ // rotation channel
				int rotation_id = BVH_CHANNEL[this->all_joints[j_c]->joint_channel[k]];
				rotation[rotation_id - 3] = frames[j + k];
				} // rotation channel
			} // per channel
		// convert to a rotation
		Cartesian3 rot_3(rotation[0], rotation[1], rotation[2]);
		rotations.push_back(rot_3);
		j += this->all_joints[j_c]->joint_channel.size();
		} // per frame
	} // loadRotationData()
