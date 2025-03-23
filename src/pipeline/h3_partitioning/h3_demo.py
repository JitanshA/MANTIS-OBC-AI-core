import numpy as np
import folium
import webbrowser
import os
import h3
from h3_partition import generate_h3_cell_pixel_mapping

# Sample image metadata
image_shape = (4, 2000, 2000)
pixel_size_m = 35
top_left_lat = 45.0
top_left_lng = -75.0
h3_res = 6  # ~1.75 km H3 cell

# Generate mock image data
_ = np.random.randint(0, 255, image_shape)

# Get H3 cells and pixel mapping
h3_cells, cell_to_pixels = generate_h3_cell_pixel_mapping(
    top_left_lat=top_left_lat,
    top_left_lng=top_left_lng,
    h3_res=h3_res,
    image_shape=image_shape,
    pixel_size_m=pixel_size_m,
)

total_mapped_pixels = sum(len(pixels) for pixels in cell_to_pixels.values())
print(f"Total mapped pixels: {total_mapped_pixels}")
print(f"Total H3 cells covering image: {len(h3_cells)}")
first_cell = next(iter(cell_to_pixels))
print(f"First H3 cell: {first_cell}")
print(f"Contains {len(cell_to_pixels[first_cell])} pixels")

# Optional: display on map
def display_h3_cells(cells, map_name="h3_image_partition"):
    first_lat, first_lng = h3.cell_to_latlng(next(iter(cells)))
    m = folium.Map(location=[first_lat, first_lng], zoom_start=11)

    for cell_id in cells:
        boundary = h3.cell_to_boundary(cell_id)
        latlng_boundary = [(lat, lng) for lat, lng in boundary]
        folium.Polygon(
            locations=latlng_boundary, color="red", fill=True, fill_opacity=0.2
        ).add_to(m)

    map_file = f"{map_name}.html"
    m.save(map_file)
    full_path = os.path.abspath(map_file)
    webbrowser.open(f"file://{full_path}")

# Show map
display_h3_cells(h3_cells)
