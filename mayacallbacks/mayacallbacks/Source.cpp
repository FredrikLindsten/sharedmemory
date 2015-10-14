// UD1414_Plugin.cpp : Defines the exported functions for the DLL application.

#include "mayaincludes.h"
#include <iostream>

using namespace std;

MCallbackId callbackId;
MCallbackIdArray callbacks;
MCallbackIdArray tempcallbacks;
MStatus res = MS::kSuccess;

void attributeChange(MNodeMessage::AttributeMessage p_Msg, MPlug &p_Plug, MPlug &p_Plug2, void *clientData)
{
	MString string;
	if (p_Msg == 2052)
		return;
	if (p_Msg == 2056){
		MFnDagNode meshNode(p_Plug.node(), &res);
		MMatrix transform = meshNode.transformationMatrix();
		string = transform.matrix[0][0];
		string += transform.matrix[0][1];
		string += transform.matrix[0][2];
		string += transform.matrix[0][3];
		string += transform.matrix[1][0];
		string += transform.matrix[1][1];
		string += transform.matrix[1][2];
		string += transform.matrix[1][3];
		string += transform.matrix[2][0];
		string += transform.matrix[2][1];
		string += transform.matrix[2][2];
		string += transform.matrix[2][3];
		string += transform.matrix[3][0];
		string += transform.matrix[3][1];
		string += transform.matrix[3][2];
		string += transform.matrix[3][3];
		MGlobal::displayInfo(string);
		return;
	}

	string = p_Msg;
	MGlobal::displayInfo(string);
}

void nameChange(MObject &node, const MString &str, void *clientData){
	MFnDagNode obj(node);
	MGlobal::displayInfo(obj.fullPathName());
}

void meshCreated(MNodeMessage::AttributeMessage p_Msg, MPlug &p_Plug, MPlug &p_Plug2, void *clientData)
{
	MString string;
	MFnMesh meshNode(p_Plug.node(), &res);
	if (res){
		MGlobal::displayInfo(meshNode.fullPathName());
		MMessage::removeCallbacks(tempcallbacks);
		callbackId = MNodeMessage::addAttributeChangedCallback(meshNode.parent(0), attributeChange);
		callbacks.append(callbackId);
		callbackId = MNodeMessage::addNameChangedCallback(meshNode.parent(0), nameChange);
		callbacks.append(callbackId);
		MFloatPointArray points;
		MSpace::Space space = MSpace::kObject;
		meshNode.getPoints(points, space);
		for (int i = 0; i < points.length(); i++){
			string = points[i].x;
			string += " ";
			string += points[i].y;
			string += " ";
			string += points[i].z;
			MGlobal::displayInfo(string);
		}

		//callback for meshchange
	}
}

void addednode(MObject &node, void *clientData){
	callbackId = MNodeMessage::addAttributeChangedCallback(node, meshCreated);
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
	// if res == kSuccess then the plugin has been loaded,
	// otherwise is has not.
	
	callbackId = MTimerMessage::addTimerCallback(5, (MTimerMessage::MElapsedTimeFunction) timer, NULL, &res);
	callbacks.append(callbackId);
	callbackId = MDGMessage::addNodeAddedCallback(addednode, "mesh");
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
}