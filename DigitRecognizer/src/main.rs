#![recursion_limit = "131"]

mod data;
mod inference;
mod model;
mod training;

use burn::{
    backend::{Autodiff, Wgpu},
    data::dataset::Dataset,
    optim::AdamConfig,
};
use model::ModelConfig;
use training::TrainingConfig;

fn main() {
    type MyBackend = Wgpu<f32, i32>;
    type MyAutoDiffBackend = Autodiff<MyBackend>;

    // let device = Default::default();
    // let model = ModelConfig::new(10, 512).init::<MyBackend>(&device);
    // println!("{}", model);

    let device = burn::backend::wgpu::WgpuDevice::default();
    let artifact_dir = "artifacts/";
    crate::training::train::<MyAutoDiffBackend>(
        artifact_dir,
        TrainingConfig::new(ModelConfig::new(10, 512), AdamConfig::new()),
        device.clone(),
    );

    crate::inference::infer::<MyBackend>(
        artifact_dir,
        device,
        burn::data::dataset::vision::MnistDataset::test()
            .get(42)
            .unwrap(),
    );
}
