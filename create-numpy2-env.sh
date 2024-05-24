set -eu

echo "fetching artifact"
cd conda
curl https://artprodeus21.artifacts.visualstudio.com/A910fa339-c7c2-46e8-a579-7ea247548706/84710dde-1620-425b-80d0-4cf5baca359d/_apis/artifact/cGlwZWxpbmVhcnRpZmFjdDovL2NvbmRhLWZvcmdlL3Byb2plY3RJZC84NDcxMGRkZS0xNjIwLTQyNWItODBkMC00Y2Y1YmFjYTM1OWQvYnVpbGRJZC85NDE1NDgvYXJ0aWZhY3ROYW1lL2NvbmRhX2FydGlmYWN0c18yMDI0MDUyMy4xMC4xX2xpbnV4XzY0X2NfY29tcGlsZXJfdmVyc2lvbjEyY3VkYV9jX2gxMzE0MjdkMmRh0/content?format=zip --output artifact.zip
unzip artifact.zip
rm artifact.zip
mv conda_artifacts_* conda_artifact
cd conda_artifact
unzip *.zip
rm *.zip
cd ../..

ARTIFACT_PATH = `pwd`/conda/conda_artifact/build_artifacts
echo "Modifying channels"
sed -i "s$  - conda-forge$  - file://$ARTIFACT_PATH\n  - conda-forge$" dependencies.yaml
rapids-dependency-file-generator

echo "To finalize, please create a new dev environment, for example with:"
echo ""
echo "conda env create --name cudf_dev --file conda/environments/all_cuda-122_arch-x86_64.yaml"
echo "pip install --force-reinstall --pre pyarrow numpy numba"
