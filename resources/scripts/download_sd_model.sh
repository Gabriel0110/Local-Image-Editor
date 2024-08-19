#!/bin/bash

"$1" -c "import torch; from diffusers import AutoPipelineForInpainting; model = AutoPipelineForInpainting.from_pretrained('runwayml/stable-diffusion-inpainting', torch_dtype=torch.float32, variant='fp32'); model.save_pretrained('$2')"