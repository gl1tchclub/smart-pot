import express from "express";

import {
  createPlant,
  getPlants,
  getPlant,
  deletePlant,
} from "../controllers/plant.js";

const router = express.Router();

router.post("/select", createPlant);
router.get("/all", getPlants);
router.get("/plant/:id", getPlant);
router.delete("/delete/:id", deletePlant);

export default router;