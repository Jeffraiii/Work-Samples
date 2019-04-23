using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BlockSpawner : MonoBehaviour
{
    public float xPosRange;
    public float zPosRange;
    private float rangeXMax;

    private float rangeXMin;

    private float rangeZMax;

    private float rangeZMin;
    
    public float yPos;

    public float xScaleMin;
    
    public float xScaleMax;
    
    public float yScaleMin;
    public float yScaleMax;
    public float zScaleMin;
    public float zScaleMax;

    public float forwardMomentum = 0;


    public float spawnInterval;

    public int spawnAmount;

    private float _spawnTime;

    public GameObject block;
    // Start is called before the first frame update
    void Start()
    {
        _spawnTime = spawnInterval;
        

    }

    // Update is called once per frame
    void Update()
    {
        rangeXMax = transform.position.x + xPosRange;
        rangeXMin = transform.position.x - xPosRange;
        rangeZMax = transform.position.z + zPosRange;
        rangeZMin = transform.position.z - zPosRange;
        spawnInterval -= Time.deltaTime;
        if (spawnInterval < 0.0f)
        {
            for (int i = 0; i < spawnAmount; i++)
            {
                GameObject temp = Instantiate(block,
                    new Vector3(Random.Range(rangeXMin, rangeXMax), yPos, Random.Range(rangeZMin, rangeZMax)),
                    Random.rotation);
                temp.transform.localScale = new Vector3(Random.Range(xScaleMin,xScaleMax), Random.Range(yScaleMin,yScaleMax), Random.Range(zScaleMin,zScaleMax));
                temp.GetComponent<Rigidbody>().AddForce(transform.forward * forwardMomentum * -1);
            }

            spawnInterval = _spawnTime;
        }
    }
}
