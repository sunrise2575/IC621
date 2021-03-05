#include <Prelude.h>

#include _CH_GLUT_
#include _CH_OBJECT_
#include _CH_PARTICLE_
#include _CH_PARTICLEMANAGER_

////////////////////////////
//	   PARTICLE DRAW      //
////////////////////////////

void glVertex3f(Vector3 points) { glVertex3f(points.x, points.y, points.z); }

////////////////////////////
//	   PARTICLE DRAW      //
////////////////////////////

#define DRAW_POINTS 0
#define DRAW_TETRAPOD 1
#define DRAW_SPHERES 2
#define DRAW_MESH 3

void DrawParticleSphere(const Particle& particle);
void DrawParticles(ParticleManager& pm, unsigned short typeIndex, unsigned char cloudForm = false, bool useDensity = true);

////////////////////////////
//       FACE DRAW        //
////////////////////////////

void DrawFace(Face3 &face); 
void DrawFace(Face3 &face, Vector3 Red, Vector3 Green, Vector3 Blue, Vector3 Alpha = 1);

////////////////////////////
//      OBJECT DRAW		  //
////////////////////////////

void DrawMesh(Object3D &object, float Red, float Green, float Blue, float Alpha = 1);
void DrawMesh(Object3D &object);

////////////////////////////////////////////////


void DrawParticleSphere(const Particle& particle)
{
	glPushMatrix();
	glTranslatef(particle.position.x, particle.position.y, particle.position.z);
	glutSolidSphere(3, 8, 6);
	glPopMatrix();
}
void DrawParticles(ParticleManager& pm, unsigned short typeIndex, unsigned char cloudForm, bool useDensity)
{
	GLfloat arMat4[] = { 0, 0.1, 0, 1 };
	void(*DrawParticle)(const Particle& particle) = NULL;

	if (cloudForm == DRAW_POINTS)
	{
		glPointSize(3);
		glBegin(GL_POINTS);
		static double StandardDensity = 0;
		if (useDensity)
		{
			unsigned int maxIndex = pm.GetParticles(typeIndex).count();
			for (unsigned int pIndex = 0; pIndex < maxIndex; pIndex++)
			{
				StandardDensity += pm.GetParticles(typeIndex)[pIndex].density;
			}
			if (maxIndex == 0) { maxIndex++; }
			StandardDensity /= maxIndex;
		}
		for (unsigned int i = 0; i < pm.GetParticles(typeIndex).count(); i++)
		{
			if (useDensity)
			{
				double StandardDensityLog;
				StandardDensityLog = (pm.GetParticles(typeIndex)[i].density / StandardDensity) - 1;
				if (StandardDensityLog > 0)
				{
					arMat4[0] = StandardDensityLog + 0.01;
					arMat4[2] = 0.01;
				}
				else
				{
					arMat4[2] = -StandardDensityLog + 0.01;
					arMat4[0] = 0.01;
				}
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, arMat4);
				glColor4fv(arMat4);
				glVertex3f(pm.GetParticles(typeIndex)[i].position);

			}
			else
			{
				arMat4[1] = 0.5;
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, arMat4);
				glColor4fv(arMat4);
				glVertex3f(pm.GetParticles(typeIndex)[i].position);
			}
		}
		glEnd();
	}
	else
	{
		switch (cloudForm)
		{
		case DRAW_SPHERES:
			DrawParticle = DrawParticleSphere;
			break;
		case DRAW_MESH:
			//not writed yet
			DrawParticle = DrawParticleSphere;
			break;
		}
		/*
		for (unsigned int i = 0; i < pm.size(); i++)
		{
			arMat4[3] = pow(pm.particleData(i).pressure / 100.0, 2) + 0.2;
			if (arMat4[3] > 0)
			{
				arMat4[0] = pm.particleData(i).pressure / 100.0 + 0.5;
				glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, arMat4);
				glColor4fv(arMat4);
				DrawParticle(pm.particleData(i));
			}
		}
		*/
	}
}

void DrawFace(Face3 &face)
{
	Vector3 tempNormal;
	tempNormal = cross(face.first - face.third, face.second - face.third);
	tempNormal /= abs(tempNormal);

	glNormal3f(tempNormal.x, tempNormal.y, tempNormal.z);
	glVertex3f(face.first.x, face.first.y, face.first.z);
	glVertex3f(face.second.x, face.second.y, face.second.z);
	glVertex3f(face.third.x, face.third.y, face.third.z);


	
}
void DrawFace(Face3 &face, Vector3 Red, Vector3 Green, Vector3 Blue, Vector3 Alpha)
{
	Vector3 tempNormal;
	tempNormal = cross(face.first - face.third, face.second - face.third);
	tempNormal /= abs(tempNormal);

	glNormal3f(tempNormal.x, tempNormal.y, tempNormal.z);

	float currentColor[4];
	glGetFloatv(GL_CURRENT_COLOR, currentColor);
	glColor4f(Red.x, Green.x, Blue.x, Alpha.x);
	GLfloat arMat4[] = { Red.x, Green.x, Blue.x, Alpha.x };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, arMat4);
	glVertex3f(face.first.x, face.first.y, face.first.z);
	glColor4f(Red.y, Green.y, Blue.y, Alpha.y);
	arMat4[0] = Red.y; arMat4[1] = Green.y, arMat4[2] = Blue.y, arMat4[3] = Alpha.y;
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, arMat4);
	glVertex3f(face.second.x, face.second.y, face.second.z);
	glColor4f(Red.z, Green.z, Blue.z, Alpha.z);
	arMat4[0] = Red.z; arMat4[1] = Green.z, arMat4[2] = Blue.z, arMat4[3] = Alpha.z;
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, arMat4);
	glVertex3f(face.third.x, face.third.y, face.third.z);
	glColor4f(currentColor[0], currentColor[1], currentColor[2], currentColor[3]);
}


void DrawMesh(Object3D &object, float Red, float Green, float Blue, float Alpha)
{
	float currentColor[4];
	glGetFloatv(GL_CURRENT_COLOR, currentColor);
	glColor4f(Red, Green, Blue, Alpha);
	GLfloat arMat4[] = { Red, Green, Blue, Alpha };
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, arMat4);
	for (unsigned int i = 0; i < object.faceSize(); i++){DrawFace(object.getVertex(i));}
	glColor4f(currentColor[0], currentColor[1], currentColor[2], currentColor[3]);
}
void DrawMesh(Object3D &object) { DrawMesh(object, 0.5, 0.5, 0.5); }