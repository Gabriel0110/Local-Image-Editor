import sys
import torch
from transformers import pipeline

def main(python_executable, model_save_path):    
    device = "cuda" if torch.cuda.is_available() else "mps" if torch.backends.mps.is_available() else "cpu"
    pipe = pipeline(task="depth-estimation", model="depth-anything/Depth-Anything-V2-Small-hf", device=device)
    pipe.save_pretrained(model_save_path)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <python_executable> <model_save_path>")
        sys.exit(1)

    main(sys.argv[1], sys.argv[2])