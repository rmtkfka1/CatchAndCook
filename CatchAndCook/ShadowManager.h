#pragma once

struct Light;
class Camera;

struct ShadowCasterParams
{
	std::array<Matrix, 4> lightViewMatrix;
	std::array<Matrix, 4> lightProjectionMatrix;
	std::array<Matrix, 4> lightVPMatrix;
	std::array<Matrix, 4> lightInvertViewMatrix;
	std::array<Matrix, 4> lightInvertProjectionMatrix;
	std::array<Matrix, 4> lightInvertVPMatrix;
	unsigned int cascadeCount = 1;
	Vector3 padding;
};
struct ShadowCascadeIndexParams
{
	unsigned int cascadeIndex = 1;
	Vector3 padding;
};


class ShadowManager : public RenderCBufferSetter
{
public:
	ShadowCasterParams _shadowCasterParams;
	ShadowCascadeIndexParams _shadowCascadeIndexParams;



	static unique_ptr<ShadowManager> main;
	ShadowManager();
	~ShadowManager();
	void Init();
	void SetShadowCasterParams();
	std::vector<BoundingFrustum> GetFrustums(Camera* camera, Light* light, const std::vector<float>& distances);

	std::vector<BoundingOrientedBox> GetBounds(Camera* camera, Light* light, const std::vector<float>& distances);
	void SetData(Material* material) override;
};

