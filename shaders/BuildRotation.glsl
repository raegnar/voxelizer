//*****************************************************************************
//BuildRotation.glsl***********************************************************
//*****************************************************************************

//const float epsilon = 0.000001f;

// 3. Tomas Moller and John F. Hughes, "Efficiently building a matrix
//    to rotate one vector to another," Journal of Graphics Tools,
//    4(4):1-4, 1999.
//mat3 buildRotation(vec3 from, vec3 to)
//{
//	float e = dot(from, to);
//	vec3  v = normalize(cross(from, to));
//
//	float h = (1.0f - e) / dot(v, v);
//
//	mat3 mat;
//
//	mat[0][0] = h*v.x*v.x + e;
//	mat[1][0] = h*v.x*v.y - v.z;
//	mat[2][0] = h*v.x*v.z + v.y;
//
//	mat[0][1] = h*v.x*v.y + v.z;
//	mat[1][1] = h*v.y*v.y + e;
//	mat[2][1] = h*v.y*v.z - v.x;
//
//	mat[0][2] = h*v.x*v.z - v.y;
//	mat[1][2] = h*v.y*v.z + v.x;
//	mat[2][2] = h*v.z*v.z + e;
//
//	return mat;
//}


//inline bool CloseEnough(float f1, float f2)
//{
////	return fabs((f1 - f2) / ((f2 == 0.f) ? 1.f : f2)) 
////		< std::numeric_limits<float>::epsilon();
////	epsilon = 0.000001f; // I think
//}

mat3 buildRotation(vec3 from, vec3 to)
{
	float e = dot(from, to);
	vec3  v = normalize(cross(from, to));

	float h = (1.0f - e) / dot(v, v);

	mat3 mat;
	if(abs(e - 1.0f) < 0.000001f)					//(e == 1.0f) vectors are the same
	{
		return mat3(1);
	}
	else if(abs(e + 1.0f) < 0.000001f)				//(e == -1.0f) vectors are the opposite
	{
		vec3 side = vec3(0.0f, from.z, -from.y);

		if(abs(dot(side, side)) < 0.000001f)		//dot(side, side) == 0.0f)
		{
			side = vec3(-from.z, 0.0f, from.x);
		}
		side = normalize(side);

		vec3 up = normalize(cross(side, from));

		mat[0][0] = -(from.x * from.x) + (up.x * up.x) - (side.x * side.x);
		mat[1][0] = -(from.x * from.y) + (up.x * up.y) - (side.x * side.y);
		mat[2][0] = -(from.x * from.z) + (up.x * up.z) - (side.x * side.z);

		mat[0][1] = -(from.x * from.y) + (up.x * up.y) - (side.x * side.y);
		mat[1][1] = -(from.y * from.y) + (up.y * up.y) - (side.y * side.y);
		mat[2][1] = -(from.y * from.z) + (up.y * up.z) - (side.y * side.z);

		mat[0][2] = -(from.x * from.z) + (up.x * up.z) - (side.x * side.z);
		mat[1][2] = -(from.y * from.z) + (up.y * up.z) - (side.y * side.z);
		mat[2][2] = -(from.z * from.z) + (up.z * up.z) - (side.z * side.z);
	}
	else
	{
		mat[0][0] = h*v.x*v.x + e;
		mat[1][0] = h*v.x*v.y - v.z;
		mat[2][0] = h*v.x*v.z + v.y;

		mat[0][1] = h*v.x*v.y + v.z;
		mat[1][1] = h*v.y*v.y + e;
		mat[2][1] = h*v.y*v.z - v.x;

		mat[0][2] = h*v.x*v.z - v.y;
		mat[1][2] = h*v.y*v.z + v.x;
		mat[2][2] = h*v.z*v.z + e;
	}
	return mat;
}
