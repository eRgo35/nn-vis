from fastapi.responses import JSONResponse
from fastapi import FastAPI, APIRouter
from callbacks import DataHolder
import numpy as np
import logging

logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s',
                    filemode='a', filename='avc.log')

logging.info("Starting server...")
logging.info("Importing libraries...")


class coreApp:
    def __init__(self, data_holder: DataHolder, datasource: np.ndarray, wrap_frames: bool = True):
        self.data_holder = data_holder
        self.router = APIRouter()

        self.router.add_api_route(
            "/values/{frame}", self.read_values, methods=["GET"])
        self.router.add_api_route(
            "/bias/{frame}", self.read_bias, methods=["GET"])
        self.router.add_api_route(
            "/weights/{frame}", self.read_weights, methods=["GET"])
        self.router.add_api_route(
            "/structure", self.read_structure, methods=["GET"])

    async def read_values(self, frame: int):
        '''
        This function returns a dictionary of layers with the values of each of the neurons.
        Frame - the frame number (sample number) from the dataset.
        '''
        if frame >= self.datasource.shape[0]:
            if self.wrap_frames:
                frame = frame % self.datasource.shape[0]
            else:
                return JSONResponse(status_code=400, content={"error": "Frame out of range"})

        layers = self.data_holder.get_layer_outputs(self.datasource[frame])
        data = {}
        for j in range(len(layers)):
            data[j] = {}
            for i in range(len(layers[j])):
                data[j][i] = layers[j][i].tolist()
        # print(data)
        return data

    async def read_bias(self, frame: int):
        '''
        This function returns a dictionary of layers with the biases of each of the neurons.
        '''
        if frame >= self.datasource.shape[0]:
            if self.wrap_frames:
                frame = frame % self.datasource.shape[0]
            else:
                return JSONResponse(status_code=400, content={"error": "Frame out of range"})

        data = {}
        for j in range(len(self.data_holder.layers)):
            data[j] = {}
            for i in range(len(self.data_holder.layers[j].bias)):
                data[j][i] = self.data_holder.layers[j].bias[i].np().tolist()
        return data

    async def read_weights(self, frame: int):
        '''
        This function returns a dictionary of layers with the weights of each of the connections.
        '''
        if frame >= self.datasource.shape[0]:
            if self.wrap_frames:
                frame = frame % self.datasource.shape[0]
            else:
                return JSONResponse(status_code=400, content={"error": "Frame out of range"})

        data = {}
        for j in range(len(self.data_holder.layers)):
            data[j] = {}
            for i in range(len(self.data_holder.layers[j].weights)):
                data[j][i] = self.data_holder.layers[j].weights[i].np().tolist()

        return data

    async def read_structure(self):
        data = {}
        data['numberOfLayers'] = len(self.data_holder.layers)
        data['input'] = self.data_holder.input_shape
        data['output'] = self.data_holder.output_shape
        data['layers'] = {
            i: {
                "position": {
                    "x": 0.0,
                    "y": 0.0,
                },
                "name": layer.name,
                "type": layer.type,
                "shape": layer.shape,
            } for i, layer in enumerate(self.data_holder.layers)
        }
        return data


if __name__ == "__main__":
    # data_holder = DataHolder()
    # x = np.sin(np.linspace(0, 1, 32*32)).reshape(-1, 32, 1)
    # app = coreApp(data_holder, x)
    app.router.include_router(app.router, prefix="/api", tags=["api"])
    # app.run(host="
