from transformers import pipeline
from PIL import Image
import requests

# load pipe
pipe = pipeline(task="depth-estimation", model="/Users/gtomberlin/Documents/Code/Local-Image-Editor/resources/models/depth-estimation")

# load image
url = 'http://images.cocodataset.org/val2017/000000039769.jpg'
image = Image.open(requests.get(url, stream=True).raw)

# inference
depth = pipe(image)["depth"]

# save image
depth.save("depth_estimated_image.png")