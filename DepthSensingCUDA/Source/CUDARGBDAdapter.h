#pragma once

#include <cuda_runtime.h> 
#include <cuda_d3d11_interop.h>
#include "cudaUtil.h"
#include "cuda_SimpleMatrixUtil.h"

#include "CUDAHoleFiller.h"
#include "CUDAScan.h"

#include "RGBDSensor.h"

#include <cstdlib>

class CUDARGBDAdapter
{
	public:

		CUDARGBDAdapter();
		~CUDARGBDAdapter();

		void OnD3D11DestroyDevice();			
		HRESULT OnD3D11CreateDevice(ID3D11Device* device, RGBDSensor* depthSensor, unsigned int width, unsigned int height);

		HRESULT process(ID3D11DeviceContext* context);

		float* getDepthMapResampledFloat()	{
			return d_depthMapResampledFloat;
		}

		float4* getColorMapResampledFloat4() {
			return d_colorMapResampledFloat4;
		}

		const mat4f& getDepthIntrinsics() const	{
			return m_depthIntrinsics;
		}

		const mat4f& getDepthIntrinsicsInv() const {
			return m_depthIntrinsicsInv;
		}

		const mat4f& getColorIntrinsics() const {
			return m_colorIntrinsics;
		}

		const mat4f& getColorIntrinsicsInv() const {
			return m_colorIntrinsicsInv;
		}

		const mat4f& getDepthExtrinsics() const	{
			return m_depthExtrinsics;
		}

		const mat4f& getDepthExtrinsicsInv() const {
			return m_depthExtrinsicsInv;
		}

		const mat4f& getColorExtrinsics() const {
			return m_colorExtrinsics;
		}

		const mat4f& getColorExtrinsicsInv() const {
			return m_colorExtrinsicsInv;
		}

		unsigned int getWidth() const {
			return m_width;
		}

		unsigned int getHeight() const {
			return m_height;
		}

		unsigned int getFrameNumber() const {
			return m_frameNumber;
		}

		RGBDSensor* getRGBDSensor() {
			return m_RGBDSensor;
		}

		// debugging
		float* getRawDepthMap() {
			return d_depthMapFloat;
		}

		mat4f getRigidTransform() const {
			return m_RGBDSensor->getRigidTransform();
		}

		//! resets the frame number
		void reset() {
			m_frameNumber = 0;
			m_RGBDSensor->reset();
		}

		void saveRecordedFramesToFile(const std::string& filename) {
			m_RGBDSensor->saveRecordedFramesToFile(filename);
		}

		void recordFrame() {
			m_RGBDSensor->recordFrame();
		}
		void recordTrajectory(const mat4f& transform) {
			m_RGBDSensor->recordTrajectory(transform);
		}

		//! record depth frame as point cloud, accumulates
		void recordPointCloud(const mat4f& transform = mat4f::identity()) {
			m_RGBDSensor->recordPointCloud(transform);
		}
		void saveRecordedPointCloud(const std::string& filename) {
			m_RGBDSensor->saveRecordedPointCloud(filename);
		}

		const mat4f& getRecordedTrajectory(unsigned int frameIdx) const {
			return m_RGBDSensor->getRecordedTrajectoryMatrix(frameIdx);
		}

		const int getNumberOfTrajectories() const{
			return m_RGBDSensor->getNumberOfTrajctories();
		}

		void storeTrajectoryIntoFile(const std::string& filename){
			m_RGBDSensor->storeTrajectoryIntoFile(filename);
		}

		void saveSinglePoseIntoFile(const std::string& filename, int frame_idx){
			m_RGBDSensor->saveSinglePoseIntoFile(filename, frame_idx);
		}

		int getNumberOfFramesReadFromRGBData() const {
			return m_RGBDSensor->getNumberOfFramesReadFromRGBData();
		}

		bool isFrameIdxInRangeOfRGBData(const unsigned int& frameIdx){
			return m_RGBDSensor->isFrameIdxInRangeOfRGBData(frameIdx);
		}

		// Copy the data from GPU to CPU and write the data (in float) into local file
		// Input:
		//    dataSize: number of T type objects
		//    elementNum: number of elements in each T type object  
		template<class T>
		inline void writeDataToLocalFile(T* inputData, unsigned int dataSize, unsigned int elementNum, std::string filename = "output.txt")
		{
			float* dataPtr = new float[dataSize*elementNum];
			cutilSafeCall(cudaMemcpy(dataPtr, inputData, dataSize * sizeof(T), cudaMemcpyDeviceToHost));

			std::ofstream writeOut(filename, std::ios::trunc);
			for (int i = 0; i < dataSize; ++i)
			{
				writeOut << dataPtr[i] << std::endl;
			}
			writeOut.close();
			SAFE_DELETE_ARRAY(dataPtr);
		}

	private:
		
		RGBDSensor*		m_RGBDSensor; // CHAO: object for depth sensor depending on the device
		unsigned int	m_frameNumber;

		mat4f m_depthIntrinsics;
		mat4f m_depthIntrinsicsInv;

		mat4f m_depthExtrinsics;
		mat4f m_depthExtrinsicsInv;

		mat4f m_colorIntrinsics;
		mat4f m_colorIntrinsicsInv;

		mat4f m_colorExtrinsics;
		mat4f m_colorExtrinsicsInv;

		unsigned int m_width;
		unsigned int m_height;

		//! depth texture float [D]
		float* d_depthMapFloat;					// float
		float* d_depthMapResampledFloat;		// re-sampled depth
	
		//! color texture float [R G B A]
		BYTE*	d_colorMapRaw;
		float4*	d_colorMapFloat4;
		float4* d_colorMapResampledFloat4;


		Timer m_timer;

};
