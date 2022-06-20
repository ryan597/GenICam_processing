"""
Web API to host current images from the R-Pi.
Should also be able to schedule acquisitions and trigger now.
Reads latest images from Genicam_processing/data and uploads.
More complex editing of parameters is to be left to ssh remote access.
"""

import os
import glob
import datetime
from flask import Flask, render_template, url_for
from PIL import Image


app = Flask(__name__)


@app.route('/')
def home():
    list_of_files = glob.glob('static/data/**/*.tiff')
    latest_file = max(list_of_files, key=os.path.getctime)
    # Must convert from tiff to png to display
    im = Image.open(latest_file)
    image_name = latest_file[6:].split('/')[-1]
    image_string = "static/" + image_name[:-4] + "png"
    im.save(image_string)

    now = datetime.datetime.now().strftime("%Y-%m-%d  %H:%M:%S")
    templateData = {
            "title": "Raspberry Pi 4b - GenICam interactive",
            "time": now,
            "image_url": url_for("static", filename=image_string[6:])
            }
    return render_template("index.html", **templateData)


if __name__ == "__main__":
    app.run(host="0.0.0.0", port=5000, debug=False)
