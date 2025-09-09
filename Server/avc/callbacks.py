import tensorflow as tf
import keras as keras
import numpy as np
import json


class DataHolder:
    def __init__(self):
        self.initialized = False
        pass

    def load_model(self, model):
        if self.initialized:
            # print("Model already loaded.")
            return
        self.model = model
        self.initialized = True

        self._create_intermediate()

    def _create_intermediate(self):
        if not self.initialized:
            raise ValueError("Model not initialized.")
        # print("Creating intermediate model")
        layers = self.model.layers
        layers_to_include = []
        for layer in layers:
            if layer.name.startswith("DNI"):
                continue
            layers_to_include.append(layer)

        # for layer in layers_to_include:
        #     print(
        #         f"Layer Name: {layer.name} Type: {layer.__class__.__name__}")
        self.layers = layers_to_include
        self.intermediate = keras.Function([layers_to_include[0].input],
                                           [l.output for l in layers_to_include[0:]])

    def _reshaper(self, x):
        """Reshapes the input data to match the intermediate model input shape."""
        if isinstance(x, list):
            x = np.array(x)
        if len(x.shape) == 1:
            x = np.expand_dims(x, axis=0)
        if len(x.shape) == 2:
            x = np.expand_dims(x, axis=-1)
        return x

    @staticmethod
    def normalize_list(x):
        min = 999999999999999
        max = -999999999999999
        for i in x:
            if isinstance(i, list):
                i = np.array(i)
            if len(i.shape) == 1:
                i = np.expand_dims(i, axis=0)
            if len(i.shape) == 2:
                i = np.expand_dims(i, axis=-1)
            min = np.min(i) if np.min(i) < min else min
            max = np.max(i) if np.max(i) > max else max
        x_range = max - min
        if x_range == 0:
            return [np.zeros_like(i) for i in x]
        return [(i - min) / x_range for i in x]

    @staticmethod
    def normalize_dict(x):
        min = 999999999999999
        max = -999999999999999
        for key in x.keys():
            i = x[key]
            if isinstance(i, list):
                i = np.array(i)
            if len(i.shape) == 1:
                i = np.expand_dims(i, axis=0)
            if len(i.shape) == 2:
                i = np.expand_dims(i, axis=-1)
            min = np.min(i) if np.min(i) < min else min
            max = np.max(i) if np.max(i) > max else max
        x_range = max - min
        if x_range == 0:
            return {key: np.zeros_like(x[key]) for key in x.keys()}
        return {key: (x[key] - min) / x_range for key in x.keys()}

    def normalize(self, x):
        """Normalizes the values in the input tensor or list. to be between -1 and 1."""
        if isinstance(x, list):
            return self.normalize_list(x)
        if isinstance(x, dict):
            return self.normalize_dict(x)
        x_min = np.min(x, -1)
        x_max = np.max(x, -1)
        x_range = x_max - x_min
        if x_range == 0:
            return np.zeros_like(x)
        return (x - x_min) / x_range

    def get_weights(self):
        if not self.initialized:
            raise ValueError("Model not initialized.")

        kernels = {}
        for layer in self.layers:
            if layer.name.startswith("NW"):
                continue
            tmp = {}
            for weight_var in layer.weights:
                if 'kernel' in weight_var.name:  # Filter only kernel weights
                    tmp[weight_var.name] = np.interp(
                        weight_var.numpy().tolist(), [-1, 1], [0, 1]
                    ).tolist()
            kernels[layer.name] = tmp

        with open("kernels_only.json", "w") as f:
            json.dump(kernels, f)
        
        return kernels

    def get_biases(self):
        if not self.initialized:
            raise ValueError("Model not initialized.")
        biases = {}
        for layer in self.layers:
            for b in layer.trainable_variables:
                if "bias" in b.name:
                    biases[layer.name] = np.interp(
                        b.numpy().tolist(), [-1, 1], [0, 1]).tolist()
        # self.normalize(biases)  # make sure this is JSON-safe too
        return biases

    def get_layer_outputs(self, x, normalize=True):
        if not self.initialized:
            raise ValueError("Model not initialized.")
        x = self._reshaper(x)
        if len(x.shape) == 3:  # Hacky fix
            x = np.expand_dims(x, axis=0)
        # Flatten the shape of x from 1, 28, 28, 1 to None, 784

        x_flat = x.reshape(x.shape[0], -1)

        layer_outputs = self.intermediate([x_flat])
        if normalize:
            layer_outputs = self.normalize(layer_outputs)
            
        return layer_outputs

    def get_input_shape(self):
        if not self.initialized:
            raise ValueError("Model not initialized.")
        return self.model.input_shape


class LayerConfigCallback(keras.callbacks.Callback):
    def __init__(self, data_holder: DataHolder):
        super().__init__()
        self.data_holder = data_holder

    def on_predict_end(self, logs=None):
        """Called at the end of a prediction step."""
        logs = logs or {}

        variables = self.model.weights
        layers = self.model.layers
        idx = 0
        # print(f"Layers {layers}")
        self.data_holder.load_model(self.model)
        for layer in layers:
            # print(
            #     f"Layer {idx} Name: {layer.name} Type: {layer.__class__.__name__}")
            weight = layer.trainable_variables
            for w in weight:
                pass
            #     print(f"Weight: {w.name} Shape: {w.shape}")
            # print(f"")
            idx += 1
        # for var in variables:
        #     print(
        #         f"Variable: {var.name} Shape: {var.shape}")

    def on_train_batch_end(self, batch, logs=None):
        self.on_predict_end(logs)


if __name__ == "__main__":
    # Example usage
    model = tf.keras.Sequential([
        # tf.keras.layers.Conv1D(10, 3, activation='relu',
        #                        input_shape=(32, 1), name="conv1d_1"),
        tf.keras.layers.Dense(64, activation='relu',
                              name="dense_1", input_shape=(28, 28, 1)),
        tf.keras.layers.Flatten(name="DNI_flatten"),
        tf.keras.layers.Dense(32, activation='relu', name="dense_3"),
        tf.keras.layers.Dense(10, activation='sigmoid', name="dense_4")
    ])

    model.compile(optimizer='adam', loss='mse')

    # Get the MNIST dataset
    data = keras.datasets.mnist.load_data()
    # Convert Y to one-hot encoding
    y = keras.utils.to_categorical(data[0][1], 10)
    # Reshape X to match the input shape
    x = data[0][0].reshape(-1, 28, 28, 1).astype('float32') / 255

    # Fit the model with the custom callback
    dh = DataHolder()
    model.fit(x, y, epochs=5, callbacks=[
              LayerConfigCallback(dh)])

    model.save_weights('mnist_model.weights.h5')

    # With a Keras function
    # get_all_layer_outputs = keras.Function([model.layers[0].input],
    #                                        [l.output for l in model.layers[1:]])
    # layer_output = get_all_layer_outputs(
    #     [x[0].reshape(1, 32, 1)])  # Reshape to match input shape

    # Get difference from the model and layer output
    # model_output = dh.get_layer_outputs(x[0:1], False)[-1]
    # # for i, output in enumerate(layer_output):
    # #     print(f"Layer {i} Shape {output.shape} output: {output}")
    # diff = model_output - layer_output[-1]
    # print(f"layer outputs {dh.get_layer_outputs(x[0:1])}")
    # print(f"biases {dh.get_biases()}")
    # print(f"Weights {dh.get_weights()}")
    # print(f"Difference: {diff}")
