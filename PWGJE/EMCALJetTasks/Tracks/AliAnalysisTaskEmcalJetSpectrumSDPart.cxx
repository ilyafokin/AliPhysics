/************************************************************************************
 * Copyright (C) 2020, Copyright Holders of the ALICE Collaboration                 *
 * All rights reserved.                                                             *
 *                                                                                  *
 * Redistribution and use in source and binary forms, with or without               *
 * modification, are permitted provided that the following conditions are met:      *
 *     * Redistributions of source code must retain the above copyright             *
 *       notice, this list of conditions and the following disclaimer.              *
 *     * Redistributions in binary form must reproduce the above copyright          *
 *       notice, this list of conditions and the following disclaimer in the        *
 *       documentation and/or other materials provided with the distribution.       *
 *     * Neither the name of the <organization> nor the                             *
 *       names of its contributors may be used to endorse or promote products       *
 *       derived from this software without specific prior written permission.      *
 *                                                                                  *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND  *
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED    *
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           *
 * DISCLAIMED. IN NO EVENT SHALL ALICE COLLABORATION BE LIABLE FOR ANY              *
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES       *
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;     *
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND      *
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       *
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS    *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                     *
 ************************************************************************************/
#include <THistManager.h>
#include <TLinearBinning.h>
#include <TCustomBinning.h>
#include <TPDGCode.h>
#include <TString.h>
#include <TVector2.h>

#include "AliAnalysisTaskEmcalJetSpectrumSDPart.h"
#include "AliAnalysisManager.h"
#include "AliEmcalMCPartonInfo.h"
#include "AliLog.h"
#include "AliMCEvent.h"
#include "AliStack.h"

ClassImp(PWGJE::EMCALJetTasks::AliAnalysisTaskEmcalJetSpectrumSDPart)

using namespace PWGJE::EMCALJetTasks;

AliAnalysisTaskEmcalJetSpectrumSDPart::AliAnalysisTaskEmcalJetSpectrumSDPart():
    AliAnalysisTaskEmcalJet(),
    AliAnalysisEmcalSoftdropHelperImpl(),
    fHistos(nullptr),
    fDoSoftDrop(true),
    fDropMass0Jets(true),
    fBeta(0),
    fZcut(0.1),
    fUseChargedConstituents(true),
    fUseNeutralConstituents(true)
{
}

AliAnalysisTaskEmcalJetSpectrumSDPart::AliAnalysisTaskEmcalJetSpectrumSDPart(const char *name):
    AliAnalysisTaskEmcalJet(name, true),
    AliAnalysisEmcalSoftdropHelperImpl(),
    fHistos(nullptr),
    fDoSoftDrop(true),
    fDropMass0Jets(true),
    fBeta(0),
    fZcut(0.1),
    fUseChargedConstituents(true),
    fUseNeutralConstituents(true)
{
    SetMakeGeneralHistograms(true);
}

AliAnalysisTaskEmcalJetSpectrumSDPart::~AliAnalysisTaskEmcalJetSpectrumSDPart()
{
}


void AliAnalysisTaskEmcalJetSpectrumSDPart::UserCreateOutputObjects()
{
    AliAnalysisTaskEmcalJet::UserCreateOutputObjects();
    fHistos = new THistManager(Form("Histos%s", GetName()));

    // Event property
    fHistos->CreateTH1("fNevents", "Number of events", 1, 0.5, 1.5);
    fHistos->CreateTH1("fHardPartonPt", "Pt of the hard parton", 1000, 0., 1000.);
    fHistos->CreateTH1("fHardGluonPt", "Pt of the hard parton", 1000, 0., 1000.);
    fHistos->CreateTH1("fHardQuarkPt", "Pt of the hard parton", 1000, 0., 1000.);
    fHistos->CreateTH1("fFracHardPartonPt", "Pt of the hard parton / pt-hard", 100, 0., 10.);
    fHistos->CreateTH1("fFracHardGluonPt", "Pt of the hard parton / pt-hard", 100, 0., 10.);
    fHistos->CreateTH1("fFracHardQuarkPt", "Pt of the hard parton / pt-hard", 100, 0., 10.);

    // Part level QA
    fHistos->CreateTH1("hPtParticleAll", "pt spectrum of all particles", 1000, 0., 1000.);
    fHistos->CreateTH1("hPtParticleCharged", "pt spectrum of charged particles", 1000, 0., 1000.);
    fHistos->CreateTH1("hPtParticleNeutral", "pt spectrum of neutral particles", 1000, 0., 1000.);
    fHistos->CreateTH1("hPtPhotons", "pt spectrum of photons", 1000, 0., 1000.);
    fHistos->CreateTH1("hPtPhotonMothers", "pt spectrum of photons which are mothers of other photons", 1000, 0., 1000.);
    fHistos->CreateTH1("hPtParticleMax", "pt of the hardest particle", 1000, 0., 1000.);
    fHistos->CreateTH1("hPtParticleMaxCharged", "pt of the hardest charged particle", 1000, 0., 1000.);
    fHistos->CreateTH1("hPtParticleMaxNeutal", "pt of the hardest neutral particle", 1000, 0., 1000.);
    fHistos->CreateTH1("hPtParticleMaxPhoton", "pt of the hardest photon particle", 1000, 0., 1000.);
    fHistos->CreateTH2("hEtaPhiParticle", "eta-phi of particles", 100, -1., 1., 100, 0, TMath::TwoPi());
    fHistos->CreateTH2("hEtaPhiCharged", "eta-phi of charged particles", 100, -1., 1., 100, 0, TMath::TwoPi());
    fHistos->CreateTH2("hEtaPhiNeutral", "eta-phi of neutral particles", 100, -1., 1., 100, 0, TMath::TwoPi());
    fHistos->CreateTH2("hEtaPhiPhoton", "eta-phi of photons", 100, -1., 1., 100, 0, TMath::TwoPi());
    fHistos->CreateTH2("hEtaPhiMaxParticle", "eta-phi of particles", 100, -1., 1., 100, 0, TMath::TwoPi());
    fHistos->CreateTH2("hEtaPhiMaxCharged", "eta-phi of charged particles", 100, -1., 1., 100, 0, TMath::TwoPi());
    fHistos->CreateTH2("hEtaPhiMaxNeutral", "eta-phi of neutral particles", 100, -1., 1., 100, 0, TMath::TwoPi());
    fHistos->CreateTH2("hEtaPhiMaxPhoton", "eta-phi of photons", 100, -1., 1., 100, 0, TMath::TwoPi());
    fHistos->CreateTH1("hNParticlesStack", "Number of particles on the stack / event", 2001, -0.5, 2000.5);
    fHistos->CreateTH1("hNParticlesPrimaryStack", "Number of phys. primary particles on the stack / event", 2001, -0.5, 2000.5);
    fHistos->CreateTH1("hNParticlesPhysPrimStack", "Number of phys. primary particles on the stack / event", 2001, -0.5, 2000.5);
    fHistos->CreateTH1("hNParticlesSecondaryStack", "Number of secondary particles on the stack / event", 2001, -0.5, 2000.5);
    fHistos->CreateTH1("hNParticlesEvent", "Number of particles / event", 301, -0.5, 300.5);
    fHistos->CreateTH1("hNChargedEvent", "Number of charged particles / event", 301, -0.5, 300.5);
    fHistos->CreateTH1("hNNeutralEvent", "Number of neutral particles / event", 301, -0.5, 300.5);
    fHistos->CreateTH1("hNPhotonsEvent", "Number of photons / event", 301, -0.5, 300.5);

    // Jet spectrum and QA
    fHistos->CreateTH1("hPtJetConstituent", "pt of all jet constituents", 1000, 0., 1000.);
    fHistos->CreateTH1("hPtJetConstituentCharged", "pt of charged jet constituents", 1000, 0., 1000.);
    fHistos->CreateTH1("hPtJetConstituentNeutral", "pt of neutral jet constituents", 1000, 0., 1000.);
    fHistos->CreateTH1("hPtJetConstituentPhoton", "pt of photon jet constituents", 1000, 0., 1000.);
    fHistos->CreateTH2("hEtaPhiJetConstituents", "eta-phi of all jet constituents", 100, -1., 1., 100, 0., TMath::TwoPi());
    fHistos->CreateTH2("hEtaPhiJetConstituentsCharged", "eta-phi of charged jet constituents", 100, -1., 1., 100, 0., TMath::TwoPi());
    fHistos->CreateTH2("hEtaPhiJetConstituentsNeutral", "eta-phi of neutral jet constituents", 100, -1., 1., 100, 0., TMath::TwoPi());
    fHistos->CreateTH2("hEtaPhiJetConstituentsPhoton", "eta-phi of photon jet constituents", 100, -1., 1., 100, 0., TMath::TwoPi());
    fHistos->CreateTH1("hJetPt", "Jet Pt spectrum", 1000, 0., 1000.);
    fHistos->CreateTH2("hJetEtaPhi", "Jet #eta and #phi", 100, -1., 1, 100, 0., TMath::TwoPi());
    fHistos->CreateTH2("hJetNEFPt", "Neutral energy fraction vs. pt", 500, 0., 500., 100, 0., 1.);
    fHistos->CreateTH2("hJetNconstPt", "Number of jet constituents vs. pt", 500, 0., 500., 100, 0., 100.);
    fHistos->CreateTH2("hJetNallPt", "Number of jet all constituents vs. pt", 500, 0., 500., 100, 0., 100.);
    fHistos->CreateTH2("hJetNchargedPt", "Number of charged jet constituents vs. pt", 500, 0., 500., 100, 0., 100.);
    fHistos->CreateTH2("hJetNneutralPt", "Number of neutral jet constituents vs. pt", 500, 0., 500., 100, 0., 100.);
    fHistos->CreateTH2("hJetNphotonsPt", "Number of photon jet constituents vs. pt", 500, 0., 500., 100, 0., 100.);
    fHistos->CreateTH1("hMaxJetPt", "Max jet Pt spectrum", 1000, 0., 1000.);
    fHistos->CreateTH2("hMaxJetEtaPhi", "Max jet #eta and #phi", 100, -1., 1, 100, 0., TMath::TwoPi());
    fHistos->CreateTH2("hMaxJetNEFPt", "Neutral energy fraction vs. pt of the max jet", 500, 0., 500., 100, 0., 1.);
    fHistos->CreateTH2("hMaxJetNconstPt", "Number of jet constituents vs. pt", 500, 0., 500., 100, 0., 100.);
    fHistos->CreateTH2("hMaxMaxJetNallPt", "Number of jet all constituents vs. pt", 500, 0., 500., 100, 0., 100.);
    fHistos->CreateTH2("hMaxJetNchargedPt", "Number of charged jet constituents vs. pt", 500, 0., 500., 100, 0., 100.);
    fHistos->CreateTH2("hMaxJetNneutralPt", "Number of neutral jet constituents vs. pt", 500, 0., 500., 100, 0., 100.);
    fHistos->CreateTH2("hMaxJetNphotonsPt", "Number of photon jet constituents vs. pt", 500, 0., 500., 100, 0., 100.);
    fHistos->CreateTH2("hPtLeading", "Pt of the leading constituent vs. jet pt", 500, 0., 500., 500, 0., 500.);
    fHistos->CreateTH2("hPtLeadingCharged", "Pt of the leading constituent (if charged) vs. jet pt", 500, 0., 500., 500, 0., 500.);
    fHistos->CreateTH2("hPtLeadingNeutral", "Pt of the leading constituent (if neutral) vs. jet pt", 500, 0., 500., 500, 0., 500.);
    fHistos->CreateTH2("hPtLeadingPhoton", "Pt of the leading constituent (if photon) vs. jet pt", 500, 0., 500., 500, 0., 500.);
    fHistos->CreateTH2("hDrLeading", "DeltaR of the leading constituent vs. jet pt", 500, 0., 500., 100, 0., 1.);
    fHistos->CreateTH2("hDrLeadingCharged", "DeltaR of the leading constituent (if charged) vs. jet pt", 500, 0., 500., 100, 0., 1.);
    fHistos->CreateTH2("hDrLeadingNeutral", "DeltaR of the leading constituent (if neutral) vs. jet pt", 500, 0., 500., 100, 0., 1.);
    fHistos->CreateTH2("hDrLeadingPhoton", "DeltaR of the leading constituent (if photon) vs. jet pt", 500, 0., 500., 100, 0., 1.);
    fHistos->CreateTH2("hEtaPhiLeading", "eta-phi of all leading constituents", 100, -1., 1., 100, 0., TMath::TwoPi());
    fHistos->CreateTH2("hEtaPhiLeadingCharged", "eta-phi of charged leading constituents", 100, -1., 1., 100, 0., TMath::TwoPi());
    fHistos->CreateTH2("hEtaPhiLeadingNeutral", "eta-phi of neutral leading constituents", 100, -1., 1., 100, 0., TMath::TwoPi());
    fHistos->CreateTH2("hEtaPhiLeadingPhoton", "eta-phi of photon leading constituents", 100, -1., 1., 100, 0., TMath::TwoPi());
    fHistos->CreateTH1("hPtNoLeading", "Jet pt for jets without leading track", 1000, 0., 1000.);
    fHistos->CreateTH2("hFailedSDConstituents", "Jets failing SoftDrop due to constituents", 300, 0., 300., 101, -0.5, 100.5);
    fHistos->CreateTH2("hFailedSDMass", "Jets failing SoftDrop due to mass", 300, 0., 300., 100, 0., 100.);
    fHistos->CreateTH1("hFailedSDFastjet", "Jets failing SoftDrop due to fastjet", 300, 0., 300.);
    fHistos->CreateTH2("hJetMassPt", "jet mass vs. pt", 500, 0., 500, 100, 0., 100.);
    fHistos->CreateTH2("hJetMassNconst", "jet mass vs N", 101, -0.5, 100.5, 100, 0., 100.);
    fHistos->CreateTH1("hJetNoMassPt", "pt of jets with mass 0", 1000, 0., 1000.);
    fHistos->CreateTH2("hJetNoMassEtaPhi", "eta-phi of jets with mass 0", 100, -1., 1., 100, 0., TMath::TwoPi());
    fHistos->CreateTH1("hJetNoMassNall", "Number of constituents for jets with mass 0", 101, -0.5, 100.5);
    fHistos->CreateTH1("hJetNoMassNcharged", "Number of constituents for jets with mass 0", 101, -0.5, 100.5);
    fHistos->CreateTH1("hJetNoMassNneutral", "Number of constituents for jets with mass 0", 101, -0.5, 100.5);
    fHistos->CreateTH1("hJetNoMassNphotons", "Number of constituents for jets with mass 0", 101, -0.5, 100.5);

    // Outlier histos
    fHistos->CreateTH1("hJetLeadingPtHard", "fraction of the leading jet pt / pt-hard", 100, 0., 10);
    fHistos->CreateTH1("hPartLeadingPtHard", "fraction of the leading particle pt / pt-hard", 100, 0., 10);
    fHistos->CreateTH1("hChargedLeadingPtHard", "fraction of the leading charged particle pt / pt-hard", 100, 0., 10);
    fHistos->CreateTH1("hNeutralLeadingPtHard", "fraction of the leading neutral pt / pt-hard", 100, 0., 10);
    fHistos->CreateTH1("hPhotonLeadingPtHard", "fraction of the leading photon pt / pt-hard", 100, 0., 10);
    

    // SoftDrop
    if(fDoSoftDrop) {
        double R = double(int(GetJetContainer("partjets")->GetJetRadius() * 1000.))/1000.;  // Save cast from float to double truncating after 3rd decimal digit
        TLinearBinning ptbinning(500, 0., 500.),
                       nsdbinning(22, -1.5, 20.5),
                       thetagbinning(11, -0.1, 1.);
        std::unique_ptr<TBinning> zgbinning(GetZgBinning(fZcut)),
                                  rgbinning(GetRgBinning(R));
        fHistos->CreateTH2("hSDZg", "Zg vs. pt", *zgbinning, ptbinning);
        fHistos->CreateTH2("hSDRg", "Rg vs. pt", *rgbinning, ptbinning);
        fHistos->CreateTH2("fSDNsd", "Nsd vs. pt", nsdbinning, ptbinning);
        fHistos->CreateTH2("fSDThetag", "Thetag vs. pt", thetagbinning, ptbinning);
    }

    for(auto h : *(fHistos->GetListOfHistograms())) fOutput->Add(h);
    PostData(1, fOutput);
}

bool AliAnalysisTaskEmcalJetSpectrumSDPart::Run()
{
    fHistos->FillTH1("fNevents", 1.);
    auto jets = GetJetContainer("partjets");
    auto particles = jets->GetParticleContainer();

    if(fMCPartonInfo) {
        auto hardest = fMCPartonInfo->GetHardestParton();
        auto hardestpt = hardest->GetMomentum().Pt();
        fHistos->FillTH1("fHardPartonPt", hardestpt);
        if(TMath::Abs(fPtHard) > 1e-5) fHistos->FillTH1("fFracHardPartonPt", hardestpt/fPtHard);
        if(hardest->GetPdg() < 7) {
            fHistos->FillTH1("fHardQuarkPt", hardestpt); 
            if(TMath::Abs(fPtHard) > 1e-5) fHistos->FillTH1("fFracHardGluonPt", hardestpt/fPtHard);
        } else {
            fHistos->FillTH1("fHardGluonPt", hardestpt);
            if(TMath::Abs(fPtHard) > 1e-5) fHistos->FillTH1("fFracHardQuarkPt", hardestpt/fPtHard);
        }
    }

    // check MC event directly
    int nphysprim(0), nsecondary(0);
    auto stack = fMCEvent->Stack();
    for(int ipart = 0; ipart < stack->GetNtrack(); ipart++) {
        if(stack->IsPhysicalPrimary(ipart)) nphysprim++;
        if(stack->IsSecondaryFromWeakDecay(ipart)) nsecondary++;

        // check couple of decays
        auto particle = stack->Particle(ipart);
        auto abspdg = TMath::Abs(particle->GetPdgCode());
        if(abspdg == kGamma) {
            bool isPhotonMother = false;
            if(particle->GetFirstDaughter() > 0 && particle->GetLastDaughter() > 0) {
                for(auto idaughter = particle->GetFirstDaughter(); idaughter <= particle->GetLastDaughter(); idaughter++) {
                    auto daughter = stack->Particle(idaughter);
                    if(TMath::Abs(daughter->GetPdgCode()) == kGamma) {
                        isPhotonMother = true;
                    }
                }
            }
            if(isPhotonMother) fHistos->FillTH1("hPtPhotonMothers", particle->Pt());
        }
    }
    fHistos->FillTH1("hNParticlesStack", fMCEvent->GetNumberOfTracks());
    fHistos->FillTH1("hNParticlesPrimaryStack", stack->GetNprimary());
    fHistos->FillTH1("hNParticlesPhysPrimStack", nphysprim);
    fHistos->FillTH1("hNParticlesSecondaryStack", nsecondary);

    int nall(0), ncharged(0), nneutral(0), nphotons(0);
    AliVParticle *maxpart(nullptr), *maxcharged(nullptr), *maxneutral(nullptr), *maxphoton(nullptr);
    for(auto part : particles->accepted()) {
        double partpt = TMath::Abs(part->Pt()),
               parteta = part->Eta(),
               partphi = TVector2::Phi_0_2pi(part->Phi());
        nall++;
        fHistos->FillTH1("hPtParticleAll", partpt);
        fHistos->FillTH2("hEtaPhiParticle", parteta, partphi);
        if(!maxpart) maxpart = part;
        else {
            if(part->Pt() > maxpart->Pt()) maxpart = part;
        }
        if(!part->Charge()) {
            // Neutral particle
            nneutral++;
            fHistos->FillTH1("hPtParticleNeutral", partpt);
            fHistos->FillTH2("hEtaPhiNeutral", parteta, partphi);
            if(!maxneutral) maxneutral = part;
            else {
                if(part->Pt() > maxneutral->Pt()) maxneutral = part;   
            }
            if(TMath::Abs(part->PdgCode()) == kGamma) {
                nphotons++;
                fHistos->FillTH1("hPtPhotons", partpt);
                fHistos->FillTH2("hEtaPhiPhoton", parteta, partphi);
                if(!maxphoton) maxphoton = part;
                else {
                    if(part->Pt() > maxphoton->Pt()) maxphoton = part;
                }
            }
        } else {
            ncharged++;
            fHistos->FillTH1("hPtParticleCharged", partpt);
            fHistos->FillTH2("hEtaPhiCharged", parteta, partphi);
            if(!maxcharged) maxcharged = part;
            else {
                if(part->Pt() > maxcharged->Pt()) maxcharged = part;
            }
        }
    }
    if(maxpart) {
        fHistos->FillTH1("hPtParticleMax", TMath::Abs(maxpart->Pt()));
        fHistos->FillTH2("hEtaPhiMaxParticle", maxpart->Eta(), TVector2::Phi_0_2pi(maxpart->Phi()));
        if(fPtHard > 1e-5) fHistos->FillTH1("hPartLeadingPtHard", maxpart->Pt() / fPtHard);
    }
    if(maxcharged) {
        fHistos->FillTH1("hPtParticleMaxCharged", TMath::Abs(maxcharged->Pt()));
        fHistos->FillTH2("hEtaPhiMaxCharged", maxcharged->Eta(), TVector2::Phi_0_2pi(maxcharged->Phi()));
        if(fPtHard > 1e-5) fHistos->FillTH1("hChargedLeadingPtHard", maxcharged->Pt() / fPtHard);
    }
    if(maxneutral) {
        fHistos->FillTH1("hPtParticleMaxNeutal", TMath::Abs(maxneutral->Pt()));
        fHistos->FillTH2("hEtaPhiMaxNeutral", maxneutral->Eta(), TVector2::Phi_0_2pi(maxneutral->Phi()));
        if(fPtHard > 1e-5) fHistos->FillTH1("hNeutralLeadingPtHard", maxneutral->Pt() / fPtHard);
    }
    if(maxphoton) {
        fHistos->FillTH1("hPtParticleMaxPhoton", TMath::Abs(maxphoton->Pt()));
        fHistos->FillTH2("hEtaPhiMaxPhoton", maxphoton->Eta(), TVector2::Phi_0_2pi(maxphoton->Phi()));
        if(fPtHard > 1e-5) fHistos->FillTH1("hPhotonLeadingPtHard", maxphoton->Pt() / fPtHard);
    }
    fHistos->FillTH1("hNParticlesEvent", nall);
    fHistos->FillTH1("hNChargedEvent", ncharged);
    fHistos->FillTH1("hNNeutralEvent", nneutral);
    fHistos->FillTH1("hNPhotonsEvent", nphotons);

    if(!jets) {
        AliErrorStream() << GetName() << ": Part. level jet container not found" << std::endl;
        return false;
    }

    Double_t vertex[3] = {0,0,0};
    AliAnalysisEmcalSoftdropHelperImpl::SoftdropParams sdsettings;
    sdsettings.fBeta = fBeta;
    sdsettings.fZcut = fZcut;
    sdsettings.fReclusterizer = AliAnalysisEmcalSoftdropHelperImpl::EReclusterizer_t::kCAAlgo;
    sdsettings.fUseChargedConstituents = fUseChargedConstituents;
    sdsettings.fUseNeutralConstituents = fUseNeutralConstituents;

    AliEmcalJet *maxjet(nullptr);
    for(auto j : jets->accepted()) {
        if(!maxjet || (j->Pt() > maxjet->Pt())) maxjet = j;
        fHistos->FillTH1("hJetPt", j->Pt());
        fHistos->FillTH2("hJetEtaPhi", j->Eta(), TVector2::Phi_0_2pi(j->Phi()));
        fHistos->FillTH2("hJetNEFPt", j->Pt(), j->NEF());
        fHistos->FillTH2("hJetNconstPt", j->Pt(), j->N());
        fHistos->FillTH1("hJetMassPt", j->Pt(), j->M());
        fHistos->FillTH1("hJetMassNconst", j->N(), j->M());
        if(TMath::Abs(j->M()) < 1e-5) {
            fHistos->FillTH1("hJetNoMassPt", j->Pt());
            fHistos->FillTH2("hJetNoMassEtaPhi", j->Eta(), j->Phi());
            int mall(0), mcharged(0), mneutral(0), mphotons(0);
            for(int ijpart = 0; ijpart < j->GetNumberOfTracks(); ijpart++) {
                auto jconst = j->Track(ijpart);
                mall++;
                if(!jconst->Charge()) {
                    mneutral++;
                    if(TMath::Abs(jconst->PdgCode()) == kGamma) mphotons++;
                } else mcharged++;
            }
            fHistos->FillTH1("hJetNoMassNall", mall);
            fHistos->FillTH1("hJetNoMassNcharged", mcharged);
            fHistos->FillTH1("hJetNoMassNneutral", mneutral);
            fHistos->FillTH1("hJetNoMassNphotons", mphotons);
        }

        // loop jet constituents
        int call(0), ccharged(0), cneutral(0), cphoton(0);
        for(auto jpart = 0; jpart < j->GetNumberOfTracks(); jpart++) {
            auto jconst = j->Track(jpart);
            double ptconst = TMath::Abs(jconst->Pt()),
                   etaconst = jconst->Eta(),
                   phiconst = TVector2::Phi_0_2pi(jconst->Phi());
            call++;
            fHistos->FillTH1("hPtJetConstituent", ptconst);
            fHistos->FillTH2("hEtaPhiJetConstituents", etaconst, phiconst);
            if(!jconst->Charge()) {
                cneutral++;
                fHistos->FillTH1("hPtJetConstituentNeutral", ptconst);
                fHistos->FillTH2("hEtaPhiJetConstituentsNeutral", etaconst, phiconst);
                if(TMath::Abs(jconst->PdgCode()) == kGamma) {
                    cphoton++;
                    fHistos->FillTH1("hPtJetConstituentPhoton", ptconst);
                    fHistos->FillTH2("hEtaPhiJetConstituentsPhoton", etaconst, phiconst);
                }
            } else {
                ccharged++;
                fHistos->FillTH1("hPtJetConstituentCharged", ptconst);
                fHistos->FillTH2("hEtaPhiJetConstituentsCharged", etaconst, phiconst);
            }
        }
        fHistos->FillTH2("hJetNallPt", j->Pt(), call);
        fHistos->FillTH2("hJetNchargedPt", j->Pt(), ccharged);
        fHistos->FillTH2("hJetNneutralPt", j->Pt(), cneutral);
        fHistos->FillTH2("hJetNphotonsPt", j->Pt(), cphoton);

        auto leading = j->GetLeadingTrack();
        if(leading) {
            double leadingpt = TMath::Abs(leading->Pt()),
                   leadingeta = leading->Eta(),
                   leadingphi = TVector2::Phi_0_2pi(leading->Phi());
            TVector3 jetvec(j->Px(), j->Py(), j->Px()),
                     leadingvec(leading->Px(), leading->Py(), leading->Pz());
            double dR = jetvec.DeltaR(leadingvec);
            fHistos->FillTH2("hPtLeading", j->Pt(), leadingpt);
            fHistos->FillTH2("hEtaPhiLeading", leadingeta, leadingphi);
            fHistos->FillTH2("hDrLeading", j->Pt(), dR);
            if(!leading->Charge()) {
                fHistos->FillTH2("hPtLeadingNeutral", j->Pt(), leadingpt);
                fHistos->FillTH2("hEtaPhiLeadingNeutral", leadingeta, leadingphi);
                fHistos->FillTH2("hDrLeadingNeutral", j->Pt(), dR);
                if(TMath::Abs(leading->PdgCode()) == kGamma) {
                    fHistos->FillTH2("hPtLeadingPhoton", j->Pt(), leadingpt);
                    fHistos->FillTH2("hEtaPhiLeadingPhoton", leadingeta, leadingphi);
                    fHistos->FillTH2("hDrLeadingPhoton", j->Pt(), dR);
                }
            } else {
                fHistos->FillTH2("hPtLeadingCharged", j->Pt(), leadingpt);
                fHistos->FillTH2("hEtaPhiLeadingCharged", leadingeta, leadingphi);
                fHistos->FillTH2("hDrLeadingCharged", j->Pt(), dR);
            }
        } else {
            fHistos->FillTH1("hPtNoLeading", j->Pt());
        }

        // SoftDrop
        if(fDoSoftDrop) {
            if(j->GetNumberOfTracks() > 1 && j->M() > 0) { // Temporary condition to prevent a crash in the declustering due to 0 jet mass
                try {
                    auto sdparams = this->MakeSoftdropStandAlone(*j, jets->GetJetRadius(), true, sdsettings, AliVCluster::VCluUserDefEnergy_t::kNonLinCorr, vertex, fDropMass0Jets);
                    bool untagged = sdparams.fZg < sdsettings.fZcut;
                    std::vector<SoftdropResults> splittings;
                    if(!untagged) {
                        // only try iterative declustering if the jet is not an untagged jet
                        splittings = this->IterativeDecluster(*j, jets->GetJetRadius(), true, sdsettings, AliVCluster::VCluUserDefEnergy_t::kNonLinCorr, vertex, fDropMass0Jets);
                    }

                    fHistos->FillTH2("hSDZg", sdparams.fZg, j->Pt());
                    fHistos->FillTH2("hSDRg", untagged ? -0.02 : sdparams.fRg, j->Pt());
                    fHistos->FillTH2("fSDNsd", untagged ? -1. : splittings.size(), j->Pt());
                    fHistos->FillTH2("fSDThetag", sdparams.fZg < sdsettings.fZcut ? -0.05 : sdparams.fRg/jets->GetJetRadius(), j->Pt());
                } catch(int errorcode) {
                    switch (errorcode)
                    {
                    case 1:
                        fHistos->FillTH2("hFailedSDConstituents", j->Pt(), j->N());
                        break;
                    case 2:
                        fHistos->FillTH2("hFailedSDMass", j->Pt(), j->M());
                        break;
                    case 3:
                    case 4:
                        fHistos->FillTH1("hFailedSDFastjet", j->Pt());
                    default:
                        break;
                    }
                } 
            }
        }
    }

    if(maxjet){
        if(fPtHard) fHistos->FillTH1("hJetLeadingPtHard", maxjet->Pt()/fPtHard);
        fHistos->FillTH1("hMaxJetPt", maxjet->Pt());
        fHistos->FillTH2("hMaxJetEtaPhi", maxjet->Eta(), maxjet->Phi());
        fHistos->FillTH2("hMaxJetNEFPt", maxjet->Pt(), maxjet->NEF());

        int nconstall = 0, nconstcharged = 0, nconstneutral = 0, nconstphoton = 0;
        for(auto ipart = 0; ipart < maxjet->GetNumberOfTracks(); ipart++) {
            auto part = maxjet->Track(ipart);
            nconstall++;
            if(!part->Charge()) {
                nconstneutral++;
                if(TMath::Abs(part->PdgCode()) == kGamma) nconstphoton++;
            } else {
                nconstcharged++;
            }
        }
        fHistos->FillTH2("hMaxJetNconstPt", maxjet->Pt(), maxjet->N());
        fHistos->FillTH2("hMaxMaxJetNallPt", maxjet->Pt(), nconstall);
        fHistos->FillTH2("hMaxJetNchargedPt", maxjet->Pt(), nconstcharged);
        fHistos->FillTH2("hMaxJetNneutralPt", maxjet->Pt(), nconstneutral);
        fHistos->FillTH2("hMaxJetNphotonsPt", maxjet->Pt(), nconstphoton);
    }

    return true;
}

AliAnalysisTaskEmcalJetSpectrumSDPart *AliAnalysisTaskEmcalJetSpectrumSDPart::AddTaskEmcalJetSpectrumSDPart(AliJetContainer::EJetType_t jettype, double R, const char *nameparticles) {
    auto mgr = AliAnalysisManager::GetAnalysisManager();
    if(!mgr) {
        AliErrorGeneralStream("AliAnalysisTaskEmcalJetSpectrumSDPart::AddTaskEmcalJetSpectrumSDPart") << "No analysis manager available" << std::endl;
        return nullptr;
    }

    TString rstring = Form("R%02d", int(R * 10.)),
            jtstring = "";

    switch (jettype) {
        case AliJetContainer::kChargedJet: jtstring = "ChargedJet"; break;
        case AliJetContainer::kFullJet:    jtstring = "FullJet"; break;
        case AliJetContainer::kNeutralJet: jtstring = "NeutralJet"; break;
        case AliJetContainer::kUndefinedJetType: break;
    };

    auto task = new AliAnalysisTaskEmcalJetSpectrumSDPart(Form("PartLevelJetTask%s%s", jtstring.Data(), rstring.Data()));
    mgr->AddTask(task);

    // Adding particle and jet container
    auto partcont = task->AddMCParticleContainer(nameparticles);
    partcont->SetMinPt(0.);
    partcont->SetParticleEtaLimits(-0.7, 0.7);
    // For purely part. level jets make acceptance radially symmetric in phi,
    // in order ot be more efficient, but restrict to eta limits of EMCAL
    auto jetcont = task->AddJetContainer(jettype, AliJetContainer::antikt_algorithm, AliJetContainer::E_scheme, R, AliJetContainer::kTPCfid, partcont, nullptr);
    jetcont->SetJetEtaLimits(-0.7 + R, 0.7 - R);
    jetcont->SetName("partjets");
    jetcont->SetMaxTrackPt(1000);
    jetcont->SetMinPt(0);
    jetcont->SetMaxPt(1000.);
    
    // Link input and output
    mgr->ConnectInput(task, 0, mgr->GetCommonInputContainer());
    mgr->ConnectOutput(task, 1, mgr->CreateContainer(Form("PartLevelJetResults%s%s", jtstring.Data(), rstring.Data()), AliEmcalList::Class(), AliAnalysisManager::kOutputContainer, mgr->GetCommonFileName()));

    return task;
}