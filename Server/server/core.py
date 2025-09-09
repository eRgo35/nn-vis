from fastapi import FastAPI

pocDemo = FastAPI()


@pocDemo.get("/values/{frame}")
def read_values(frame: int):
    '''
    This function returns a dictionary of layers with the values of each of the neurons.
    Frame - the frame number added to the data.
    '''
    data = {}
    for j in range(3):
        data[j] = {}
        for i in range(9):
            data[j][i] = -i/10 + frame/100
    return data


@pocDemo.get("/bias/{frame}")
def read_bias(frame: int):
    '''
    This function returns a dictionary of layers with the biases of each of the neurons.
    '''
    data = {}
    for j in range(3):
        data[j] = {}
        for i in range(9):
            data[j][i] = (i + frame) % 20
    return data


@pocDemo.get("/weights/{frame}")
def read_weights(frame: int):
    '''
    This function returns a dictionary of layers with the weights of each of the connections.
    '''
    data = {}
    for k in range(2):
        data[k] = {}
        for j in range(9):
            data[k][j] = {}
            for i in range(9):
                data[k][j][i] = float(frame)/100.0

    return data


@pocDemo.get("/structure")
def read_structure():
    data = {}
    data['numberOfLayers'] = "3"
    data['input'] = "9"
    data['input_shape'] = "3x3"
    data['output'] = "9"
    data['output_shape'] = "3x3"
    data["layers"] = {
        0: {
            "position": {
                "x": 0.0,
                "y": 0.0,
                "z": 0.0
            },
            "type": "dense",
            "height": 3,
            "width": 3,
        },
        1: {
            "position": {
                "x": 0.0,
                "y": 100.0,
                "z": 0.0
            },

            "type": "dense",
            "height": 3,
            "width": 3,
        },
        2: {
            "position": {
                "x": 0.0,
                "y": 200.0,
                "z": 0.0
            },

            "type": "dense",
            "height": 3,
            "width": 3,
        }
    }

    return data
