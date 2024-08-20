import sys
import torch
from diffusers import StableDiffusionInpaintPipeline

def main(python_executable, model_save_path):
    device = "cuda" if torch.cuda.is_available() else "mps" if torch.backends.mps.is_available() else "cpu"
    model = StableDiffusionInpaintPipeline.from_pretrained(
        "stabilityai/stable-diffusion-2-inpainting",
        torch_dtype=torch.float32 if device in ["cpu", "mps"] else torch.float16,
        safety_checker=None)
    model.save_pretrained(model_save_path)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <python_executable> <model_save_path>")
        sys.exit(1)

    main(sys.argv[1], sys.argv[2])
