#pragma once

#include <cuda_runtime.h> 
#include <cuda_d3d11_interop.h>
#include "cudaUtil.h"
#include "cuda_SimpleMatrixUtil.h"

#include "CUDAHoleFiller.h"
#include "CUDAScan.h"

#include "RGBDSensor.h"

#include <cstdlib>

/**
 * CUDARGBDAdapter
 * A wrapper class implemented in CUDA to adapt the sensor to a user specific output size.
 * It re-samples the original sensor output to a desired size and also adjusted the intrinsic matrix.
 * Note that the depth and color map data provided by this adapter lives on GPU.
 *
 * Another feature provided by CUDARGBDAdaptor is called batch buffering. The basic idea is to buffer
 * some number of frames on the GPU so that the scheduler can easily perform optimizations such as 
 * loop reordering, fusing and so forth.
 */

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

		mat4f getRigidTransform(int offset = 0) const {
			return m_RGBDSensor->getRigidTransform(offset);
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

		int getCurrentSensorIdx() {
			return m_RGBDSensor->getCurrentSensorIdx();
		}

		//! record depth frame as point cloud, accumulates
		void recordPointCloud(const mat4f& transform = mat4f::identity()) {
			m_RGBDSensor->recordPointCloud(transform);
		}
		void saveRecordedPointCloud(const std::string& filename) {
			m_RGBDSensor->saveRecordedPointCloud(filename);
		}

	private:

		void updateCameraMatrices();
		void allocateGPUBuffers(BYTE** pp_colorMapRaw, float4** pp_colorMapFloat4,
												 float4** pp_colorMapResampledFloat4, float** pp_depthMapFloat,
												 float** pp_depthMapResampledFloat);


		void resampleDepthColor(BYTE* colorMapRaw, 
								float4* colorMapFloat4, 
								float4* colorMapResampledFloat4, 
								float* d_depthMapFloat, 
								float* depthMapResampledFloat);

	private:
		unsigned int m_width;
		unsigned int m_height;
		RGBDSensor*	 m_RGBDSensor;
		unsigned int m_frameNumber;

		float* d_depthMapFloat;
		float* d_depthMapResampledFloat;
		BYTE*	d_colorMapRaw;
		float4*	d_colorMapFloat4;
		float4* d_colorMapResampledFloat4;

		// Camera intrinsics and extrinsics
		mat4f m_depthIntrinsics;
		mat4f m_depthIntrinsicsInv;
		mat4f m_depthExtrinsics;
		mat4f m_depthExtrinsicsInv;
		mat4f m_colorIntrinsics;
		mat4f m_colorIntrinsicsInv;
		mat4f m_colorExtrinsics;
		mat4f m_colorExtrinsicsInv;

		Timer m_timer;
};
