// UD1414_Plugin.cpp : Defines the exported functions for the DLL application.

#include "mayaincludes.h"
#include <iostream>
#include "server.h"
#include "structs.h"

using namespace std;

MCallbackId callbackId;
MCallbackIdArray callbacks;
MCallbackIdArray tempcallbacks;
MStatus res = MS::kSuccess;
MSpace::Space space = MSpace::kObject;
Server* server;
int index = 0;
char* buffer = new char[1 << 20];

void meshMoved(MNodeMessage::AttributeMessage p_Msg, MPlug &p_Plug, MPlug &p_Plug2, void *clientData)
{
	if (p_Msg == 2052)
		return;
	if (p_Msg == 2056){
		Mesh* mesh = (Mesh*)clientData;
		memcpy(buffer + 12, &mesh->index, 4);
		memcpy(buffer + 16, mesh->node->transformationMatrix().matrix, 128);
		server->WriteMessage(buffer, 144, MeshMoved);
		return;
	}
	MString string;
	string = p_Plug.name();
	MGlobal::displayInfo(string);
}

void meshChanged(MNodeMessage::AttributeMessage p_Msg, MPlug &p_Plug, MPlug &p_Plug2, void *clientData)
{
	MString string;
	if (p_Msg == 2052)
		return;
	int vertIndex = p_Plug.logicalIndex();
	if (p_Msg == 2056){
		if (vertIndex<0)
			return;
		Mesh* mesh = (Mesh*)clientData;
		double test = p_Plug.asDouble();
		MPoint pos;
		mesh->mesh->getPoint(vertIndex, pos, space);
		memcpy(buffer + 12, &mesh->index, 4);
		memcpy(buffer + 16, &vertIndex, 4);
		memcpy(buffer + 24, &pos.x, 24);
		server->WriteMessage(buffer, 48, MeshChange);
		return;
	}
}

void nameChange(MObject &node, const MString &str, void *clientData){
	MFnDagNode* mesh = (MFnDagNode*)clientData;
	int length = mesh->name().length();
	memcpy(buffer + 12, mesh->name().asChar(), length);
	server->WriteMessage(buffer, length + 12, MeshNameChange);
}

void meshCreated(float elapsedTime, float lastTime, void *clientData)
{
	MObject* node = (MObject*)clientData;
	MFnDagNode dagnode(*node, &res);
	int error = res.statusCode();
	MFnMesh* mesh = new MFnMesh(*node);
	error = res.statusCode();
	if (res){

		MGlobal::displayInfo(mesh->fullPathName());
		MeshData mesh_data;
		MFloatPointArray points;
		MFloatVectorArray normals;

		mesh_data.id = index;
		mesh_data.name = mesh->name();

		MString command = "polyTriangulate -ch 1 " + mesh_data.name;
		MGlobal::displayInfo(command);
		MGlobal::executeCommand(command);

		MStringArray uvSets;
		mesh->getUVSetNames(uvSets);

		uvSet tempUVSet;
		MFloatArray Us;
		MFloatArray Vs;
		vec2 UVs;

		MItMeshPolygon itFaces(mesh->object(), &res);
		while (!itFaces.isDone()) {
			face tempface;
			int vc = itFaces.polygonVertexCount();
			if (vc > 3)
				return;
			for (int i = 0; i < vc; ++i) {
				tempface.verts[i].pointID = itFaces.vertexIndex(i);
				tempface.verts[i].normalID = itFaces.normalIndex(i);

				for (uint k = 0; k < uvSets.length(); ++k) {
					int temptexCoordsID;
					itFaces.getUVIndex(i, temptexCoordsID, &uvSets[k]);

					tempface.verts[i].texCoordsID.push_back(temptexCoordsID);
				}
			}
			mesh_data.faces.push_back(tempface);
			itFaces.next();
		}

		size_t size = 12;

		mesh->getPoints(points, space);
		for (uint i = 0; i < points.length(); i++){
			vec3 temppoints = { points[i].x, points[i].y, points[i].z };
			mesh_data.points.push_back(temppoints);
		}

		mesh->getNormals(normals, space);
		for (uint i = 0; i < normals.length(); i++){
			vec3 tempnormals = { normals[i].x, normals[i].y, normals[i].z };
			mesh_data.normals.push_back(tempnormals);
		}

		for (uint i = 0; i < uvSets.length(); i++)
		{
			MString currentSet = uvSets[i];
			mesh->getUVs(Us, Vs, &currentSet);
			for (uint a = 0; a < Us.length(); a++){
				UVs.u = Us[a];
				UVs.v = 1.0f - Vs[a];
				tempUVSet.UVs.push_back(UVs);
			}
			mesh_data.uvSets.push_back(tempUVSet);
		}


		MeshHeader* header = (MeshHeader*)(buffer+12);
		header->id = index;
		header->nameLength = mesh_data.name.length();
		header->pointCount = mesh_data.points.size();
		header->normalCount = mesh_data.normals.size();
		header->uvCount = mesh_data.uvSets[0].UVs.size();
		header->faceCount = mesh_data.faces.size();
		size += sizeof(MeshHeader);
		
		memcpy(buffer + size, mesh_data.name.asChar(), mesh_data.name.length());
		size += mesh_data.name.length();

		memcpy(buffer + size, mesh_data.points.data(), mesh_data.points.size());
		size += mesh_data.points.size();

		memcpy(buffer + size, mesh_data.normals.data(), mesh_data.normals.size());
		size += mesh_data.normals.size();

		memcpy(buffer + size, mesh_data.uvSets[0].UVs.data(), mesh_data.uvSets[0].UVs.size());
		size += mesh_data.uvSets[0].UVs.size();

		memcpy(buffer + size, mesh_data.faces.data(), mesh_data.faces.size());
		size += mesh_data.faces.size();

		server->WriteMessage(buffer, (int)size, NewMesh);

		//callback for meshchange
		Mesh* newMesh = new Mesh;
		newMesh->mesh = mesh;
		newMesh->node = new MFnDagNode(mesh->parent(0));
		newMesh->index = index;

		index++;
		MMessage::removeCallbacks(tempcallbacks);
		callbackId = MNodeMessage::addAttributeChangedCallback(mesh->parent(0), meshMoved, newMesh);
		callbacks.append(callbackId);
		callbackId = MNodeMessage::addAttributeChangedCallback(*node, meshChanged, newMesh);
		callbacks.append(callbackId);
		callbackId = MNodeMessage::addNameChangedCallback(mesh->parent(0), nameChange, newMesh->node);
		callbacks.append(callbackId);

	}
}

void addednode(MObject &node, void *clientData){
//	callbackId = MNodeMessage::addAttributeChangedCallback(node, meshCreated);
	MObject *obj = new MObject;
	*obj = node;
	callbackId = MTimerMessage::addTimerCallback(0.1, (MTimerMessage::MElapsedTimeFunction) meshCreated, obj, &res);
	tempcallbacks.append(callbackId);
	return;
}

void timer(float elapsedTime, float lastTime, void *clientData)
{
	MString string;
	string = elapsedTime;
	MGlobal::displayInfo(string);
	return;
}

// called when the plugin is loaded
EXPORT MStatus initializePlugin(MObject obj)
{
	// most functions will use this variable to indicate for errors
	MFnPlugin myPlugin(obj, "Maya plugin", "1.0", "Any", &res);
	if (MFAIL(res))  {
		CHECK_MSTATUS(res);
	}
	MGlobal::displayInfo("Maya plugin loaded!");

	server = new Server(100);

	//	callbackId = MTimerMessage::addTimerCallback(5, (MTimerMessage::MElapsedTimeFunction) timer, NULL, &res);
	//	callbacks.append(callbackId);
	callbackId = MDGMessage::addNodeAddedCallback(addednode, "mesh", server);
	callbacks.append(callbackId);

	return res;
}


EXPORT MStatus uninitializePlugin(MObject obj)
{
	// simply initialize the Function set with the MObject that represents
	// our plugin
	MFnPlugin plugin(obj);

	// if any resources have been allocated, release and free here before
	// returning...
	MMessage::removeCallbacks(callbacks);
	MGlobal::displayInfo("Maya plugin unloaded!");

	return MS::kSuccess;

	//delete
}