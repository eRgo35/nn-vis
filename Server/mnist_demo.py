from fastapi import FastAPI
from avc.callbacks import DataHolder
from fastapi.responses import JSONResponse
import numpy as np
import json
import tensorflow as tf
from tensorflow.keras import layers, models
from tensorflow.keras.datasets import mnist
from tensorflow.keras.utils import to_categorical
from pydantic import BaseModel
from typing import List

# # Load and preprocess the MNIST dataset
(x_train, y_train), (x_test, y_test) = mnist.load_data()
x_train = x_train.reshape(
    (x_train.shape[0], 28, 28, 1)).astype('float32') / 255
x_test = x_test.reshape((x_test.shape[0], 28, 28, 1)).astype('float32') / 255
y_train = to_categorical(y_train, 10)
y_test = to_categorical(y_test, 10)

# # Build the model
model = tf.keras.Sequential([
    # tf.keras.layers.Conv1D(10, 3, activation='relu',
    #                        input_shape=(32, 1), name="conv1d_1"),
    tf.keras.layers.Flatten(name="DNI_flatten", input_shape=(28, 28, 1)),
    tf.keras.layers.Dense(64, activation='relu',
                          name="NW_dense_1"),
    tf.keras.layers.Dense(32, activation='relu', name="dense_2"),
    tf.keras.layers.Dense(10, activation='sigmoid', name="dense_3")
])
model.load_weights('mnist_model.weights.h5')


# # Compile the model
# model.compile(optimizer='adam',
#               loss='categorical_crossentropy',
#               metrics=['accuracy'])

# # Train the model
# model.fit(x_train, y_train, epochs=5, batch_size=64, validation_split=0.1)

# model.save_weights('mnist_model.weights.h5')

# # Evaluate the model
# test_loss, test_acc = model.evaluate(x_test, y_test)
# print(f"Test accuracy: {test_acc:.4f}")

mnist_demo = FastAPI()
dh = DataHolder()
dh.load_model(model)


@mnist_demo.get("/structure")
def read_structure():
    data = {}
    data['numberOfLayers'] = "3"
    data['input'] = "784"
    data['input_shape'] = "28x28"
    data['output'] = "10"
    data['output_shape'] = "1x10"
    data["layers"] = {
        0: {
            "position": {
                "x": 0.0,
                "y": 0.0,
                "z": 0.0
            },
            "type": "dense",
            "height": 8,
            "width": 8,
        },
        1: {
            "position": {
                "x": 0.0,
                "y": 100.0,
                "z": 0.0
            },

            "type": "dense",
            "height": 8,
            "width": 4,
        },
        2: {
            "position": {
                "x": 0.0,
                "y": 200.0,
                "z": 0.0
            },

            "type": "dense",
            "height": 10,
            "width": 1,
        }
    }

    return data


@mnist_demo.get("/values/{frame}")
def read_values(frame: int):
    '''
    This function returns a dictionary of layers with the values of each of the neurons.
    Frame - the frame number added to the data.
    '''
    if frame >= x_train.shape[0]:
        if dh.wrap_frames:
            frame = frame % x_train.shape[0]
        else:
            return JSONResponse(status_code=400, content={"error": "Frame out of range"})

    layers = dh.get_layer_outputs(x_train[frame])
    data = {}

    for jdx, j in enumerate(layers):
        temp = j.numpy()

        if temp.ndim == 1:
            # Map index to value explicitly for 1D array
            tempdict = {idx: float(val) for idx, val in enumerate(temp)}
        else:
            # Flatten and enumerate multi-dimensional arrays
            flat = temp.flatten()
            tempdict = {idx: float(val) for idx, val in enumerate(flat)}

        data[jdx] = tempdict

    return data


@mnist_demo.get("/bias/{frame}")
def read_bias(frame: int):
    '''
    This function returns a dictionary of layers with the biases of each of the neurons.
    '''
    biases = dh.get_biases()
    data = {}

    for jdx, j in enumerate(biases.values()):
        # Flatten j if it's a nested list
        flat = [item for sublist in j for item in (
            sublist if isinstance(sublist, list) else [sublist])]
        tempdict = {idx: float(val) for idx, val in enumerate(flat)}
        data[jdx] = tempdict

    return data


@mnist_demo.get("/weights/{frame}")
def read_weights(frame: int):
    '''
    This function returns a dictionary of layers with the weights of each of the connections.
    '''

    if frame >= x_train.shape[0]:
        if dh.wrap_frames:
            frame = frame % x_train.shape[0]
        else:
            return JSONResponse(status_code=400, content={"error": "Frame out of range"})

    weights = dh.get_weights()

    data = {}

    for idx, i in enumerate(weights.values()):
        kernel = i.get("kernel")
        if kernel is not None:
            tempdict = {}
            for jdx, j in enumerate(kernel):
                tempdict[jdx] = {}
                for kdx, k in enumerate(j):
                    tempdict[jdx][kdx] = k
        
        
        data[idx] = tempdict
    print(data)
    return data



class DrawRequest(BaseModel):
    data: List[List[float]]


@mnist_demo.post("/values_draw")
def read_values_draw(dataStruct: DrawRequest):
        '''
        This function returns a dictionary of layers with the values of each of the neurons.
        Frame - the frame number added to the data.
        '''
        data = dataStruct.data
        print(data)
        data = np.array(data, dtype=np.float32)
        if data.shape != (28, 28):
            raise ValueError(f"Expected input shape (28,28), got {data.shape}")

        # Add channel dimension -> (28, 28, 1)
        data = np.expand_dims(data, axis=-1)

        # Add batch dimension -> (1, 28, 28, 1)
        data = np.expand_dims(data, axis=0)
        layers = dh.get_layer_outputs(data)
        data_return = {}

        for jdx, j in enumerate(layers):
            temp = j.numpy()

            if temp.ndim == 1:
                # Map index to value explicitly for 1D array
                tempdict = {idx: float(val) for idx, val in enumerate(temp)}
            else:
                # Flatten and enumerate multi-dimensional arrays
                flat = temp.flatten()
                tempdict = {idx: float(val) for idx, val in enumerate(flat)}

            data_return[jdx] = tempdict
        print(data_return)
        return data_return

if __name__ == "__main__":
    mnist_demo.run(debug=True)
