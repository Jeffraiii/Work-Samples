using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;
using UnityEngine.UI;

public class GameManager : MonoBehaviour
{
    public static List<GameObject> wells = new List<GameObject>();


    public GameObject imageObject;
    public Image imageToFill;

    public GameObject player;

    public float shieldRechargeTime;

    private float currValue;

    private float shieldRechargeTimeStart;

    private PlayerMove moveScript;
    private float fillPercentage;
    private float fillAmount = 0;



    // Start is called before the first frame update
    void Start()
    {
        imageObject = GameObject.Find("RechargeCircle");
        imageToFill = imageObject.GetComponent<Image>();
        player = GameObject.Find("Player");
        moveScript = player.GetComponent<PlayerMove>();
        currValue = 0.0f;
        shieldRechargeTime = moveScript.shieldCooldown;
    }

    // Update is called once per frame
    void Update()
    {

        if (moveScript._isShieldOnCooldown)
        {
            currValue = Time.time - shieldRechargeTimeStart;
            fillPercentage = currValue / shieldRechargeTime;

            //fillAmount = Mathf.Clamp01(fillPercentage);

            imageToFill.fillAmount = fillPercentage;
        }
        else
        {
            currValue = 0;
            shieldRechargeTimeStart = Time.time;
        }
    }

  
}
