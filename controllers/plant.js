import { PrismaClient, Prisma } from "@prisma/client";
const prisma = new PrismaClient();

const createPlant = async (req, res) => {
  try {
    // Validate the content-type request header. It ensures that the request body is in JSON format
    const contentType = req.headers["content-type"];
    if (!contentType || contentType !== "application/json") {
      return res.status(400).json({
        msg: "Invalid Content-Type. Expected application/json.",
      });
    }

    // const { name, daysWater, threshold }

    await prisma.plant.create({
        data: {
          name: req.body.name,
          institutionId: req.body.institutionId,
        },
      });

  } catch (error) {
    if (err instanceof Prisma.PrismaClientKnownRequestError) {
      {
        if (err.code === "P2002") {
          return res.status(400).json({
            msg: "Plant with the same name already exists",
          });
        }
      }
    }
  }
};

const getPlants = async (req, res) => {
  try {
    const plants = await prisma.plant.findMany();

    // Check if there are no plants
    if (!plants) {
      return res.status(404).json({ msg: "No plants found" });
    }

    return res.status(200).json({
      data: plants,
    });
  } catch (err) {
    return res.status(500).json({
      msg: err.message,
    });
  }
};

const getPlant = async (req, res) => {
  try {
    const plant = await prisma.plant.findUnique({
      where: { id: Number(req.params.id) },
    });

    // Check if there is no plant
    if (!plant) {
      return res
        .status(404)
        .json({ msg: `No plant with the id: ${req.params.id} found` });
    }

    return res.status(200).json({
      data: plant,
    });
  } catch (err) {
    return res.status(500).json({
      msg: err.message,
    });
  }
};

const deletePlant = async (req, res) => {
  try {
    const plant = await prisma.plant.findUnique({
      where: { id: Number(req.params.id) },
    });

    if (!plant) {
      return res
        .status(404)
        .json({ msg: `No plant with the id: ${req.params.id} found` });
    }

    await prisma.plant.delete({
      where: { id: Number(req.params.id) },
    });

    return res.json({
      msg: `Plant with the id: ${req.params.id} successfully deleted`,
    });
  } catch (err) {
    return res.status(500).json({
      msg: err.message,
    });
  }
};

export { createPlant, getPlants, getPlant, deletePlant };
