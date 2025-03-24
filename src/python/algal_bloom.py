import numpy as np
import joblib
import os
import pandas as pd
from typing import Tuple, Dict, Any

# Features expected by the model
features = [
    "B02_mean", "B03_mean", "B04_mean", "B08_mean",
    "green_red_ratio", "green_blue_ratio", "red_blue_ratio",
    "green95th", "green5th",
    "green95th_blue_ratio", "green5th_blue_ratio",
    "NDVI_B04", "percent_water"
]

# Load model
MODEL_PATH = os.path.join(os.path.dirname(os.path.abspath(__file__)), "models", "lgbm_model.pkl")
_MODEL = joblib.load(MODEL_PATH)

def compute_features_for_cell(
    image: np.ndarray,
    pixel_indices: Tuple[np.ndarray, np.ndarray]
) -> Dict[str, Any]:
    rows, cols = pixel_indices
    red = image[0][rows, cols].astype(np.float32)
    green = image[1][rows, cols].astype(np.float32)
    blue = image[2][rows, cols].astype(np.float32)
    nir = image[3][rows, cols].astype(np.float32)
    eps = 1e-6

    green_red_ratio = np.mean(green / (red + eps))
    green_blue_ratio = np.mean(green / (blue + eps))
    red_blue_ratio = np.mean(red / (blue + eps))

    green95th = np.percentile(green, 95)
    green5th = np.percentile(green, 5)
    green95th_blue_ratio = green95th / (np.mean(blue) + eps)
    green5th_blue_ratio = green5th / (np.mean(blue) + eps)

    ndvi_vals = (nir - red) / (nir + red + eps)
    ndvi = np.mean(ndvi_vals)

    water_mask = ndvi_vals < 0.1
    percent_water = np.sum(water_mask) / len(ndvi_vals) # TODO: Update this

    return {
        "B02_mean": float(np.mean(blue)),
        "B03_mean": float(np.mean(green)),
        "B04_mean": float(np.mean(red)),
        "B08_mean": float(np.mean(nir)),
        "green_red_ratio": float(green_red_ratio),
        "green_blue_ratio": float(green_blue_ratio),
        "red_blue_ratio": float(red_blue_ratio),
        "green95th": float(green95th),
        "green5th": float(green5th),
        "green95th_blue_ratio": float(green95th_blue_ratio),
        "green5th_blue_ratio": float(green5th_blue_ratio),
        "NDVI_B04": float(ndvi),
        "percent_water": float(percent_water),
    }

def predict_hab_for_cell(
    image: np.ndarray,
    pixel_indices: Tuple[np.ndarray, np.ndarray]
) -> float:
    """
    Predicts HAB bloom probability for a single water cell.
    """
    features_dict = compute_features_for_cell(image, pixel_indices)
    input_df = pd.DataFrame([features_dict], columns=features) 
    return float(_MODEL.predict_proba(input_df)[0][1]) # TODO: CUDA this 