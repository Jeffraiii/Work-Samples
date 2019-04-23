using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class InitWell : MonoBehaviour
{
    //public GameManager GameManager;
    // Start is called before the first frame update
    public GameObject emitter;
    public bool explode = false;
    private float t = 1.0f;
    void Start()
    {
        GameManager.wells.Add(gameObject);
        emitter.SetActive(false);
    }

    private void OnDestroy()
    {
        GameManager.wells.Remove(gameObject);
    }

    // Update is called once per frame
    void Update()
    {
        if (explode)
        {
            t -= Time.deltaTime;
            if (t < 0.0f)
            {
                Destroy(gameObject);
            }
        }
    }
}
