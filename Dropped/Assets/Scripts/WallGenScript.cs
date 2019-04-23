using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class WallGenScript : MonoBehaviour
{

    public int width = 6;
    public int height = 5;

    public float xSpawn = 0;
    public float ySpawn = 5;

    //private int currW = 0;
    //private int currH = 0;


    public GameObject block;
    // Start is called before the first frame update
    void Start()
    {
        //createWall();
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void createWall()
    {

        for(int i=0; i<height; ++i)
        {
            for(int j=0; j<width; ++j)
            {

                float xPos = xSpawn + j * 2;
                float yPos = transform.position.y + ySpawn + (height * i) + (j*1.5f) ;
                float zPos = transform.position.z;
                GameObject temp = Instantiate(block, new Vector3(xPos, yPos, zPos), Quaternion.identity );

            }
        }
    }
}
